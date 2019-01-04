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
#include "CFTaskHeader.h"
#include "TimeUtilities.h"

namespace wjp::cf{
class Task;
// TaskBlock is a 128-bytes fix-sized memory block 
// that comprises a TaskHeader, a Task and padding bytes.
// All TaskBlocks are compactly allcoated in TaskDeques or TaskBuffers.
class TaskBlock{
public:
    TaskBlock() noexcept{
        if constexpr(MeasureInitTime){
            static wjp::time_point start=now();
            static int i=0;
            i++;
            if(i==DequeCapacity) std::cout<<"time elapsed="<<wjp::ms_elapsed_count(start)<<std::endl;
        }
    }
    TaskHeader&                 taskHeader() noexcept{
        return header;
    }
    void*                       taskAddress() noexcept{
        return address;
    }
    template < class T, class... Args >
    T*                          emplaceTask(Args&&... args){
        return new (address) T(std::forward<Args>(args)...);
    }
    template < class T = Task >
    T*                          taskPointer()noexcept{
        return reinterpret_cast<T*>(address);
    }
    template < class T = Task >
    T*                          taskReference()noexcept{
        return *reinterpret_cast<T*>(address);
    }
    template < class T >
    void                        destroyTask(){
        reinterpret_cast<T*>(address)->~T();
    }
    void                        setIndex(uint8_t index)noexcept{
        header.emplacerIndex=index;
    }
private:
    TaskHeader                  header; 
    char                        address[CacheLineSize-sizeof(TaskHeader)]; // padded to 128 bytes
};


}