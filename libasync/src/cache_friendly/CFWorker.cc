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
 * Worker routine fetches one task each iteration.
 * It searches a task in the following order:
 * 1. if execList is nonempty, run a task fetched from execList (newest first)
 * 2. elif readyList is nonempty, run a task fetched from readyList (newest first) 
 * 3. elif there exist tasks in other workers' readLists, run a task stolen from others.(FIFO)
 * 4. elif there exist a task in local submission buffer, run it. (FIFO)    
 * 5. elif there exist a task in other workers' submission buffer, run it. (FIFO)
 * 6. else search fails.
 */
void Worker::routine()
{
    // Seach a task
     // [1] execList
    Task* task = deque.takeFromExec();
    if(task==nullptr){
       // [2] readyList 
       task=deque.take(); 
    //    if(task==nullptr){
    //         // [3] others' readyList 
    //         task=stealFromDeque();
    //         if(task==nullptr){
    //             // [4] local buffer
    //             task=buffer.steal();
    //             if(task==nullptr){
    //                 // [5] others' buffer
    //                 task=stealFromBuffer(); 
    //             }       
    //         }   
    //     }
    }
    if(task==nullptr) return;
    // Execute the task
    if constexpr(EnableAfterDeathShortcut){
        Task* next = task->execute();
        while(next!=nullptr){
            next = task->execute();
        }
    }else{
        task->execute();
    }
}

Task* Worker::stealFromBuffer()
{
    Task* task=nullptr;
    for(uint8_t i=0;i<WorkerNumber;i++){
        if(i==index) continue;
        Worker& worker = TaskPool::instance().getWorker(i);
        task=worker.buffer.steal();
        if(task!=nullptr) return task;
    }
    return task;
}

Task* Worker::stealFromBufferOf(Worker& worker)
{
    return worker.buffer.steal();
}

Task* Worker::stealFromDeque()
{
    Task* task=nullptr;
    for(uint8_t i=0;i<WorkerNumber;i++){
        if(i==index) continue;
        Worker& worker = TaskPool::instance().getWorker(i);
        task=worker.deque.steal();
        if(task!=nullptr) return task;
    }
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
    ss<<", deque freeListSize="<<deque.freeListSize()<<", buffer size="<<buffer.size();
    return ss.str();
}

}