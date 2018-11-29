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

namespace wjp{

WorkStealingWorker::WorkStealingWorker(WorkStealingWorkerPool& pool, int index) : 
    index(index), pool(pool), deque(std::make_unique<ChaseLevDeque<Task>>()), 
    buffer(std::make_unique<SubmissionBuffer<Task>>())
{}

void WorkStealingWorker::routine(){
    auto task=scan_next_task(); 
    if(task==nullptr){  
        if(!when_idle_begins.has_value()){ 
            becomes_idle(); 
        }else{ 
            if(idle_for_too_long()){ 
                try_to_block();
            }
        }
    }else{ 
        becomes_busy(); 
        task->execute(); 
    }
}

void WorkStealingWorker::try_to_block(){
    // todo
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
    WorkStealingWorkerPool& mypool=pool.get(); // Note: auto mypool=pool.get() constructs a WorkStealingWorkerPool!
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
    return target.buffer->steal();
}

}