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

#include "CFWorker.h"
#include "ConcurrentPrint.h"
#include "CFTaskHeader.h"
#include "CFTask.h"

#include <sstream>

namespace wjp::cf{

/*
 * Cache Friendly Work Stealing Scheduling Algorithm
 * 
 * It searches a task in the following order:
 * 1. if execList is nonempty, run a task fetched from execList (newest first)
 * 2. elif readyList is nonempty, run a task fetched from readyList (newest first) 
 * 3. elif there exist tasks in other workers' readLists, run a task stolen from others.(FIFO)
 * 4. elif there exist a task in local submission buffer, run it. (FIFO)    
 * 5. elif there exist a task in other workers' submission buffer, run it. (FIFO)
 * 6. else search fails, triggers yield on idle algorithm
 * 
 * Priciples: 
 *      1. parallel DFS: new>old, better temporal locality
 *      2. avoid rampant stealing: local>steal, better spatial locality
 */

void Worker::findAndRunATaskOrYield()
{
    if(!findAndRunATask()){
       if constexpr(SimplePause){ 
            // We use a very simple(and fair) strategy 
            // to yield cpu when task becomes idle 
            wjp::sleep(100); 
            pauseCount++;
        }else{
            // Todo: use a sophiscated idle cpu yielding strategy here.
        }
        if constexpr(VerboseDebug && SimplePause){
            if(pauseCount%100==0)
                println("Worker"+std::to_string(index)+" pauseCount="+std::to_string(pauseCount));
        }
    }
}

// called by Task::sync()
bool Worker::findAndRunATask()
{
    Task* task=nullptr;
    task = takeFromLocalExecList();         
    if(task) return executeTask(task);
    task = takeFromLocalReadyList();        
    if(task) return executeTask(task);
    task = stealFromOtherDeques();                
    if(task) return executeTask(task);
    task = stealFromLocalBuffer();          
    if(task) return executeTask(task);
    task = stealFromOtherBuffers();              
    if(task) return executeTask(task);
    return false;    
}

// Note that if TaskList is not lock-free, and executeTask() does not yield cpu ownership at all, the thread that runs the current task can always generate new tasks to readylist which prevents other workers from stealing. 
// Therefore readylist must be lock-free. 
bool Worker::executeTask(Task* task)
{
    if(task==nullptr) return false;
    // To be done: check if the task is runnable; 
    // Now we assume every task user gives us is good!
    if constexpr(VeryVerboseDebug){
        println("Worker"+std::to_string(index)+" runs "+task->stats());
    }
    //wjp::sleep(10);
    if constexpr(SimplePause) pauseCount=0;
    // Execute the task
    if constexpr(EnableAfterDeathShortcut){
        Task* next = task->execute();
        while(next!=nullptr){
            next = next->execute();
        }
    }else{
        task->execute();  
    }
    return true;
}

Task* Worker::takeFromLocalReadyList(){
    if constexpr(VeryVerboseDebug){
        Task* t=deque.take();
        if(t) println("A task is taken from local readylist! "+t->stats());
        return t;
    }
    return deque.take();
}

Task* Worker::takeFromLocalExecList(){
    if constexpr(VeryVerboseDebug){
        Task* t=deque.takeFromExec();
        if(t) println("A task is taken from local execlist! "+t->stats());
        return t;
    }
    return deque.takeFromExec();
}

Task* Worker::stealFromLocalBuffer(){
    if constexpr(VeryVerboseDebug){
        Task* t=buffer.steal();
        if(t) println("A task is stolen from local buffer! "+t->stats());
        return t;
    }
    return buffer.steal();
}

Task* Worker::stealFromOtherBuffers()
{
    Task* task=nullptr;
    for(uint8_t i=0;i<WorkerNumber;i++){
        if(i==index) continue;
        Worker& worker = TaskPool::instance().getWorker(i);
        task=stealFromBufferOf(worker);
        if(task!=nullptr){
            if constexpr(VeryVerboseDebug){
                println("A task is stolen from worker"+std::to_string(i)+"'s buffer! "+task->stats());
            }
            return task;
        } 
    }
    return task;
}

Task* Worker::stealFromBufferOf(Worker& worker)
{
    return worker.buffer.steal();
}

Task* Worker::stealFromOtherDeques()
{
    Task* task=nullptr;
    for(uint8_t i=0;i<WorkerNumber;i++){
        if(i==index) continue;
        Worker& worker = TaskPool::instance().getWorker(i);
        task=stealFromDequeOf(worker);
        if(task!=nullptr){ 
            if constexpr(VeryVerboseDebug){
                println("A task is stolen from worker"+std::to_string(i)+"'s Deque! "+task->stats());
            }            
            return task;
        }
    }
    return task;
}

Task* Worker::stealFromDequeOf(Worker& worker)
{
    return worker.deque.steal();
}

void Worker::reclaim(Task* executed) noexcept
{
    if(executed->taskHeader().state==TaskHeader::StolenFromBuffer){
        buffer.reclaim(executed);
    }else{
        deque.reclaim(executed);
    }
}


std::string Worker::stat()
{
    std::stringstream ss;
    ss<<"worker "<<(int)index;
    #ifdef EnableWorkerSleep
    bool is_idle_=is_idle();
    ss<<": is_blocked="<< blocked <<", is_idle="<<is_idle_;
    if(is_idle_){
        ss<<"("<< ms_elapsed_count(when_idle_begins.value()) <<"ms)";
    }
    #endif
    ss<<", deque stats: "<<deque.stats()<<", buffer size="<<buffer.size();
    return ss.str();
}

}