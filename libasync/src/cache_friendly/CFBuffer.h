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
#include "CFBufferTaskBlock.h"

namespace wjp::cf{
class Task;

// TaskBuffer is a cache-friendly worker-local container of pre-constructed TaskBlocks.
// It holds externally submitted tasks. It's much smaller than TaskDeque.
class TaskBuffer{
public:
    using BufferMutex = std::mutex; 
    TaskBuffer(){}
    // Return the least recently emplaced task
    Task*                       steal()noexcept;
    int                         size() const noexcept;
    void                        reclaim(Task*)noexcept;
    template < class T, class... Args >  
    T*                          emplace(Args&&... args){
        std::lock_guard<BufferMutex> lk(mtx);
        if(endPosition-beginPosition >= BufferCapacity){
            return nullptr; // return nullptr if current size == capacity
        }
        BufferTaskBlock& block=at(endPosition);
        if(block.taskHeader().state==TaskHeader::StolenFromBuffer){
            return nullptr;  
        }
        T* task = block.emplaceTask<T>(std::forward<Args>(args)...);
        task->taskHeader().state=TaskHeader::Ready;
        endPosition++;
        return task;
    }
    void                        setIndex(uint8_t workerIndex)noexcept{ 
        for(auto& b : blocks) b.setIndex(workerIndex);
    }    
protected:
    // Random access should only be used internally under lock protection
    BufferTaskBlock&            at(uint64_t index) noexcept{
        return blocks[index%BufferCapacity];
    }
private:
    BufferTaskBlock             blocks[BufferCapacity];  // pre-constructed empty TaskBlocks
    mutable BufferMutex         mtx;
    uint64_t                    beginPosition=0; 
    uint64_t                    endPosition=0; // always >= beginPosition
};

}