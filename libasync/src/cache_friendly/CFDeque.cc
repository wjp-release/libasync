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

#include "CFDeque.h"
#include "CFTask.h"
#include "CFTaskHeader.h"
#include <sstream>
#include <string>

namespace wjp::cf{

// Public methods 

/* 
    Release an executed task or cancel a ready task.
    Called by onComputeDone() of each task.
    You can also call it to cancel a task in readylist.
*/
void TaskDeque::reclaim(Task* executed)noexcept{
    std::lock_guard<DequeMutex> lk(mtx);
    TaskHeader& header=executed->taskHeader();
    if(header.state==TaskHeader::Stolen){
        stolenList.remove(executed); // stealers do not clean up emplacer's stolenList
    }else if(header.state==TaskHeader::Exec){
        execList.remove(executed); // it's probably already removed except for shortcuts
    }else if(header.state==TaskHeader::Ready){
        readyList.remove(executed); // cancel task
    }else if(header.state==TaskHeader::Free){
        return;
    }else{
        assert(false && "This task should not be reclaimed by a deque!");
    }
    freeList.pushBack(executed);
    header.state=TaskHeader::Free;
}

/*
    Take the oldest task from readylist, put it into stolenlist.
    Note that this task will not be put into stealer's execlist.
    The stealer must execute stolen task immediately. 
    Return nullptr if readylist is empty.
*/
Task* TaskDeque::steal()noexcept{
    std::lock_guard<DequeMutex> lk(mtx);
    if(readyList.empty()) return nullptr;
    return toStolen(readyList.popFront());
}

/*
    Take the newest task from readylist, put it into execlist.
    <take> itself does not execute the task.
    It's called by worker routine that intends to execute the task very soon.
    Return nullptr if readylist is empty.
*/
Task* TaskDeque::take()noexcept{
    std::lock_guard<DequeMutex> lk(mtx);
    if(readyList.empty()) return nullptr;
    return toExec(readyList.popBack());
}

/*
    Collect debugging stats 
*/
std::string TaskDeque::stats()const{
    std::stringstream ss;
    ss<<"CFDeque: freelist size="<<freeListSize()<<", stolenlist size="<<stolenListSize()<<", readylist size="<<readyListSize()<<", execlist size="<<execListSize();
    ss<<std::endl;
    return ss.str();
}

// Private methods 

Task* TaskDeque::toExec(Task* task)noexcept{
    task->taskHeader().state=TaskHeader::Exec;
    execList.pushBack(task);
    return task;
}

Task* TaskDeque::toStolen(Task* task)noexcept{
    task->taskHeader().state=TaskHeader::Stolen;
    stolenList.pushBack(task);
    return task;
}

Task* TaskDeque::toReady(Task* task)noexcept{
    task->taskHeader().state=TaskHeader::Ready;
    readyList.pushBack(task);
    return task;
}

Task* TaskDeque::toFree(Task* task)noexcept{
    task->taskHeader().state=TaskHeader::Free;
    freeList.pushBack(task);
    return task;
}

void* TaskDeque::freeToReadyAddr(){
    if(freeList.empty()) throw TooManyTasks();   
    return toReady(freeList.popBack());
}

void* TaskDeque::freeToExecAddr(){
    if(freeList.empty()) throw TooManyTasks();  
    return toExec(freeList.popBack());
}

}