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
#include <atomic>

namespace wjp::cf{

class Task;
class TaskBlock;
class BufferTaskBlock;

class TaskHeader{
public:
    TaskHeader(): refCount(0){}
    enum : uint16_t {
        Free    = 0,    // not created, in freelist or buffer
        Ready   = 1,    // created, not executed, in readylist
        Exec    = 2,    // sched for execution, in local execlist
        Stolen  = 3,    // sched for execution, NOT in stealer's execlist, but is executed by stealer
        StolenFromBuffer = 10, // externally-emplaced root task that has been sched for execution
    };
    Task*                       parent=nullptr;   // 8 bytes
    Task*                       next=nullptr;     // 8 bytes
    Task*                       prev=nullptr;     // 8 bytes 
    std::atomic<uint32_t>       refCount;         // 4 bytes | The number of pending child tasks to sync with.
    uint16_t                    state=Free;       // 2 bytes
    uint8_t                     stealerIndex=0;   // 1 byte | Tasks whose state is Stolen should have a valid stealerIndex. Note that StolenFromBuffer is not Stolen.
    uint8_t                     emplacerIndex=0;  // 1 byte | Used for memory reclaimation
    bool                        isRoot=false;
    TaskBlock*                  taskBlockPointer() noexcept{
        return reinterpret_cast<TaskBlock*>(this);
    }
    BufferTaskBlock*            bufferTaskBlockPointer() noexcept{
        return reinterpret_cast<BufferTaskBlock*>(this);
    }
    template< class T >
    T*                          taskPointer() noexcept{
        return reinterpret_cast<T*>(this+1);
    }
    template< class T >
    T&                          taskReference() noexcept{
        return *reinterpret_cast<T*>(this+1);
    }                 
};

}