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

#pragma once

#include "CFConfig.h"
#include "CFTaskBlock.h"
#include <mutex> 
#include "CFTaskList.h"
#include "CFTooManyTasks.h"

namespace wjp::cf{
class Task;


/*
    TaskDeque is structured as an array of TaskBlocks.
    TaskBlocks pack TaskHeader, Task and padding bytes together in 128 bytes.
    TaskHeaders contain intrusive link nodes that help construct 3 lists: freelist, readylist, execlist
    The freelist is a list of empty tasks that have not been created yet.  
    The readylist is a list of tasks that have been created, but not executed yet.
    The execlist is a list of tasks that are being executed now.

    TaskDeque operations:
    1. <take>
    Take the newest task from readylist, put it into execlist.
    <take> itself does not execute the task.
    It's called by worker routine that intends to execute the task very soon.

    2. <steal>
    Take the oldest task from readylist, put it into execlist.
    <steal> itself does not execute the task.
    It's called by worker routine that intends to execute the task very soon.

    3. <emplace>
    Take a task from freelist, put it into readylist,
    and then create the task in-place.
*/


class TaskDeque{
public:
    using DequeMutex = std::mutex; 
    TaskDeque();
    Task*                       steal();
    Task*                       take();
    // Number of tasks that have been created (Ready or Exec)
    int                         size() const noexcept {
        return DequeCapacity-freeList.size();
    }
    template < class T, class... Args >  
    T*                          emplace(Args&&... args){
        std::lock_guard<DequeMutex> lk(mtx);
        return new (freeToReady()) T(std::forward<Args>(args)...);
    }
    template < class T, class... Args >  
    T*                          emplaceAsExec(Args&&... args){
        std::lock_guard<DequeMutex> lk(mtx);
        return new (freeToExec()) T(std::forward<Args>(args)...);
    }
protected: 
    void*                       freeToReady();
    void*                       freeToExec();
private:
    TaskBlock                   blocks[DequeCapacity];  // pre-constructed empty TaskBlocks
    mutable DequeMutex          mtx;
    TaskList                    freeList;  
    TaskList                    readyList; 
    TaskList                    execList;  
};

}