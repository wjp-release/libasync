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

namespace wjp::cf{

TaskDeque::TaskDeque(){
    for(auto& block : blocks){
        freeList.pushBack(block.taskPointer());
    }
}

/*
    Take the oldest task from readylist, put it into execlist.
    <steal> itself does not execute the task.
    It's called by worker routine that intends to execute the task very soon.
*/
Task*                       TaskDeque::steal(){
    std::lock_guard<DequeMutex> lk(mtx);
    return nullptr;
}

/*
    Take the newest task from readylist, put it into execlist.
    <take> itself does not execute the task.
    It's called by worker routine that intends to execute the task very soon.
*/
Task*                       TaskDeque::take(){
    std::lock_guard<DequeMutex> lk(mtx);
    return nullptr;
}

/*
    Return the address of the yet-to-be-created task
    Take a task from freelist, put it into readylist
*/    
void*                       TaskDeque::freeToReady()
{
    if(freeList.empty()) throw TooManyTasks();  
    Task* task = freeList.popBack();
    task->taskHeader().state=TaskHeader::Ready;
    readyList.pushBack(task);
    return task;
}


/*
    Return the address of the yet-to-be-created task
    Take a task from freelist, put it into execlist
*/    
void*                       TaskDeque::freeToExec()
{
    if(freeList.empty()) throw TooManyTasks();  
    Task* task = freeList.popBack();
    task->taskHeader().state=TaskHeader::Exec;
    execList.pushBack(task);
    return task;
}

}