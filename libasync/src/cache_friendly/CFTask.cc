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

#include "CFTask.h"
#include "CFPool.h"
#include "CFBufferTaskBlock.h"
#include "ConcurrentPrint.h"

namespace wjp::cf{

// Help or busy wait until refcnt decreases to 0.
// Helping to avoid losing concurrency: worker's routine will get executed during sync 
void Task::sync()
{
    auto index=TaskPool::instance().currentThreadIndex();
    if(index.has_value()){ // called by a worker thread
        localSync(TaskPool::instance().getWorker(index.value()));
    }else{ // called by a user thread
        externalSync();
    }
}

void Task::localSync(Worker& worker)
{
    TaskHeader& header=taskHeader();
    while(stillPending()){
        worker.findAndRunATask(); 
    }
}
// note that refCount==0 doesn't always guarantee that the task is done, since it also holds true if the task has not spawned any child task yet.
bool Task::stillPending(){
    return !taskHeader().isDone;
}

void Task::signal(){
    TaskHeader& header=taskHeader();
    if(header.isRoot){
        if constexpr(VerboseDebug) wjp::println("Signal Root Task!");
        BufferTaskBlock* block=header.bufferTaskBlockPointer();
        block->getConVar().notify_all();
    }else{
        // do nothing if the parent task is not a root task
    }
}

void Task::externalSync(){
    TaskHeader& header=taskHeader();
    if(header.isRoot){
        if constexpr(VerboseDebug) wjp::println("Root blocked! refcnt="+std::to_string(taskHeader().refCount));
        BufferTaskBlock* block=header.bufferTaskBlockPointer();
        std::unique_lock<std::mutex> lk(block->getMutex());
        while(stillPending()){
            block->getConVar().wait(lk);
        }
    }else{
        assert(false && "externSync() can only be called on a root task stolen from a buffer which has a mtx and a convar.");
    }
}

// Run user-defined routine and release the task.
Task* Task::execute(){
    Task* shortcut = compute();
    onComputeDone();
    return shortcut;
}

void Task::onComputeDone(){
    TaskHeader& header=taskHeader();
    Task* parent=header.parent;
    if(parent) parent->decRefCount();
    TaskPool::instance().getWorker(header.emplacerIndex).reclaim(this);
}

}