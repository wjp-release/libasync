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
#include "CFTaskList.h"
#include "CFTooManyTasks.h"
#include <mutex> 
#include <string>

namespace wjp::cf{
class Task;


/*
    TaskDeque is structured as an array of TaskBlocks.
    TaskBlocks pack TaskHeader, Task and padding bytes together in 128 bytes.
    TaskHeaders contain intrusive link nodes that help construct 4 lists: 
    freelist, readylist, execlist, stolenlist

    The freelist is a list of empty tasks that have not been created yet.  
    It serves as owning worker's memory pool for task construction.
    
    The readylist is a list of tasks that have been created, but not executed yet.
    It serves as the level2 scheduler search list, vulnerable to stealers. 

    The execlist is a list of tasks that are being executed now.
    It serves as the level1 scheduler search list, safe from stealing.  
    It must be small.

    The stolenlist is a list of tasks that are stolen by other workers.
    Note that the stolen tasks still physically lie in this deque; it's just executed elsewhere.
*/


class TaskDeque{
public:
    using DequeMutex = std::mutex; 
    TaskDeque()noexcept{
        for(auto& b : blocks) freeList.pushBack(b.taskPointer());
    }
    void                        reclaim(Task* executed)noexcept; 
    Task*                       take()noexcept; // return nullptr on failure
    Task*                       steal()noexcept; // return nullptr on failure
    Task*                       prefetch()noexcept;  // return nullp             tr on failure
    Task*          takeFromExec()noexcept;  // return nullptr on failure
    bool                        existsExec() const noexcept{
        return !execList.empty();
    }
    bool                        existsReady() const noexcept{
        return !readyList.empty();
    }
    bool                        existsFree() const noexcept{
        return !freeList.empty();
    }
    bool                        existsStolen() const noexcept{
        return !stolenList.empty();
    }
    int                         freeListSize() const noexcept {
        return freeList.size();
    }
    int                         stolenListSize() const noexcept {
        return stolenList.size();
    }
    int                         readyListSize() const noexcept {
        return readyList.size();
    }
    int                         execListSize() const noexcept {
        return execList.size();
    }
    int                         NumberOfEmplacedTasks() const noexcept{
        return DequeCapacity-freeList.size();
    }
    template < class T, class... Args >  
    T*                          emplace(Args&&... args){
        std::lock_guard<DequeMutex> lk(mtx);
        return new (freeToReadyAddr()) T(std::forward<Args>(args)...);
    }
    template < class T, class... Args >  
    T*                          emplaceAsExec(Args&&... args){
        std::lock_guard<DequeMutex> lk(mtx);
        return new (freeToExecAddr()) T(std::forward<Args>(args)...);
    }
    void                        setIndex(uint8_t workerIndex)noexcept{ 
        for(auto& b : blocks) b.setIndex(workerIndex);
    }    
    std::string                 stats()const;
private:
    void*                       freeToReadyAddr();
    void*                       freeToExecAddr();
    Task*                       toExec(Task* task)noexcept;
    Task*                       toStolen(Task* task)noexcept;
    Task*                       toReady(Task* task)noexcept;
    Task*                       toFree(Task* task)noexcept;
    TaskBlock                   blocks[DequeCapacity];  // pre-constructed empty TaskBlocks
    mutable DequeMutex          mtx;
    TaskList                    freeList;  
    TaskList                    readyList; 
    TaskList                    execList;      
    TaskList                    stolenList;     
};

}