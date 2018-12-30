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

namespace wjp::cf{
class Task;

// TaskDeque is a cache-friendly worker-local container of pre-constructed TaskBlocks.
// It holds internally-generated tasks. 
// It works as a memory pool for Task allocation as well. 
class TaskDeque{
public:
    using DequeMutex = std::mutex; 
    TaskDeque(){}
    // Return the least recently emplaced task
    Task*                       steal();
    // Return the most recently emplaced task
    Task*                       take();
    // Return the number of emplaced tasks
    int                         size();
    // We construct tasks in-place, so there is no need to copy or move them into deque.
    // T is a subclass of Task; args are T constructor's parameters.
    template < class T, class... Args >  
    T*                          emplaceTask(Args&&... args){
        std::lock_guard<DequeMutex> lk(mtx);
        if(endPosition-beginPosition >= DequeCapacity){
            return nullptr; // return nullptr if current size == capacity
        }
        T* task = at(endPosition).emplaceTask<T>(std::forward<Args>(args)...);
        endPosition++;
        return task;
    }
protected:
    // Random access should only be used internally under lock protection
    TaskBlock&                  at(uint64_t index){
        return blocks[endPosition%DequeCapacity];
    }
private:
    TaskBlock                   blocks[DequeCapacity];  // pre-constructed empty TaskBlocks
    mutable DequeMutex          mtx;
    uint64_t                    beginPosition=0; 
    uint64_t                    endPosition=0; // always >= beginPosition
};

}