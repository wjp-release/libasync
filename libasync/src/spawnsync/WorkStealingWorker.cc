/*
* MIT License
*
* Copyright (c) 2018 jipeng wu
* <recvfromsockaddr at gmail dot com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "WorkStealingWorker.h"
#include "WorkStealingWorkerPool.h"
#include <iostream>
#include <cassert>
#include <sstream>
#include "TimeUtilities.h"

namespace wjp{

WorkStealingWorker::WorkStealingWorker(WorkStealingWorkerPool& pool, int index) : 
    index(index), pool(pool), deque(std::make_unique<ChaseLevDeque<Task>>()), 
    buffer(std::make_unique<SubmissionBuffer<Task>>())
{}

void WorkStealingWorker::spawn(std::shared_ptr<Task> task){
    deque->push(task);
    // Aggressive wake-up strategy 
    // Since spawn usually indicates influx of child tasks,
    // now it's time to wake up all blocking workers. 
    pool.get().wake_all_sleeping_workers();   
}

void WorkStealingWorker::submit(std::shared_ptr<Task> task){
    buffer->submit(task);
    #ifdef SAMPLE_DEBUG
    println("worker "+std::to_string(index)+" submit!");   
    #endif
    if(blocked) wake();
}

void WorkStealingWorker::wake_if_blocked()
{
    if(blocked){
        blocked=false;
        cv.notify_all();
    }
}

void WorkStealingWorker::wake()
{
    #ifdef SAMPLE_DEBUG
    println("AWAKEN! >> "+stat());
    #endif
    blocked=false;
    cv.notify_all();
}

std::string WorkStealingWorker::stat(){
    std::stringstream ss;
    bool is_idle_=is_idle();
    ss<<"worker "<<index<<": is_blocked="<< blocked <<", is_idle="<<is_idle_;
    if(is_idle_){
        ss<<"("<< ms_elapsed_count(when_idle_begins.value()) <<"ms)";
    }
    ss<<", deque size="<<deque->size()<<", buffer size="<<buffer->size();
    return ss.str();
}

// The join_routine() does not take newly submitted tasks from submission buffer, 
// as its purpose is to accelerate completion of the joined parent task. 
void WorkStealingWorker::join_routine(){
    std::shared_ptr<Task> task=deque->take();  
    if(task==nullptr){
        WorkStealingWorkerPool& mypool=pool.get(); 
        task = steal_from(mypool.get_worker(who_steals_from_me_index).get());
        if(task==nullptr){
            task= steal_a_task();
            if(task==nullptr) return;
        }
    }
    task->execute(); 
}

void WorkStealingWorker::routine(){
    if(blocked){
        std::unique_lock<std::mutex> lk(mtx);
        while(blocked) cv.wait(lk);
        return;
    }
    auto task=scan_next_task(); 
    if(task==nullptr){  
        if(!is_idle()){ 
            becomes_idle(); 
        }else{ 
            if(idle_for_too_long()){ 
                try_to_block();
            }
        }
    }else{ 
        becomes_busy(); 
        #ifdef SAMPLE_DEBUG
        time_point start=now();
        println("worker "+std::to_string(index)+" starts to run task");
        #endif
        task->execute(); 
        #ifdef SAMPLE_DEBUG
        int time_elasped=ms_elapsed_count(start);
        println("worker "+std::to_string(index)+" completes task | time_elasped="+std::to_string(time_elasped)+"ms");
        #endif
    }
}

void WorkStealingWorker::try_to_block(){
    blocked=true;  
    when_idle_begins.reset(); 
}

bool WorkStealingWorker::idle_for_too_long(){
    assert(when_idle_begins.has_value());
    return ms_elapsed(when_idle_begins.value())  >  idle_timeout ;
}

std::shared_ptr<Task> WorkStealingWorker::scan_next_task(){
    auto task=deque->take();  
    if(task==nullptr){
        return steal_a_task();
    }else{
        return task;
    }
}

std::shared_ptr<Task> WorkStealingWorker::steal_a_task(){
    std::shared_ptr<Task> task=buffer->steal();
    if(task!=nullptr) return task;
    WorkStealingWorkerPool& mypool=pool.get(); 
    int cap=mypool.nr_threads();
    for(int pos=index; pos<index+cap ;pos++){
        task=steal_from(mypool.get_worker(pos%cap).get());
        if(task!=nullptr) return task;
    }
    return nullptr;
}

std::shared_ptr<Task> WorkStealingWorker::steal_from(WorkStealingWorker& target){
    auto t=target.deque->steal();
    if(t!=nullptr) return t;
    target.who_steals_from_me_index=index;  
    return target.buffer->steal();
}

// Block Immunity Strategy
bool WorkStealingWorker::immune_to_block()const noexcept{
    if(index<1) return true;  //Only the first worker is always online. Others could block.
    return false;
}

}