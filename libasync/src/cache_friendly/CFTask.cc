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
    while(header.refCount>0){
        worker.findAndRunATask(); 
    }
}

void Task::externalSync(){


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