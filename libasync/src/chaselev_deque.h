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

#include <cstdint>
#include <atomic>

// take & push are 
// steal could be invoked by any thread
// see https://www.dre.vanderbilt.edu/~schmidt/PDF/work-stealing-dequeue.pdf
// and https://www.di.ens.fr/~zappa/readings/ppopp13.pdf

#define EMPTY -1
#define ABORT -1

namespace wjp{
//template<typename T>

class chaselev_deque
{
public:
    chaselev_deque() : array(new Array(1<<13)),top(1<<12),bottom(1<<12){}

    // push, grow, take could only be invoked by the deque's owning thread
    struct Array{
        Array(uint64_t cap) : buffer(new std::atomic<int>[1<<cap]), capacity(cap) {}
        ~Array(){delete[]buffer;}
        std::atomic<int>*       buffer;
        std::atomic<uint64_t>   capacity; //power of 2, grow at rate of *2 each time, init: 2^13(8K), max: 2^26(64M)
    };

    struct autorelease{ // even though atomic ops are noexcept 
        autorelease(Array*arr):arr(arr){}
        Array* arr;
        ~autorelease(){
            delete arr;
        }
    };

    void grow(){
        auto old_arr = array.load(std::memory_order_relaxed);
        uint64_t new_capacity=old_arr->capacity<<1;
        autorelease{old_arr};
        auto new_arr=new Array(new_capacity);
        array.store(new_arr, std::memory_order_relaxed);
        for(uint64_t i=top.load(std::memory_order_relaxed);i<bottom.load(std::memory_order_relaxed);i++)  
        {
            new_arr->buffer[i].store(old_arr->buffer[i].load(std::memory_order_relaxed) , std::memory_order_relaxed);
        }
    }
    int take() {
        auto b=bottom.load(std::memory_order_relaxed)-1;
        auto a=array.load(std::memory_order_relaxed);
        bottom.store(b,std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto t=top.load(std::memory_order_relaxed);
        int x;
        if (t <= b) { /* Non-empty queue. */
            x  = a->buffer[b % a->capacity].load(std::memory_order_relaxed);
            if (t == b) { /* Single last element in queue. */
                if(!top.compare_exchange_strong(t,t+1,std::memory_order_seq_cst, std::memory_order_relaxed)){
                    x=EMPTY;
                }
                bottom.store(b+1, std::memory_order_relaxed);
            }
        } else { /* Empty queue. */
            x = EMPTY;
            bottom.store(b+1, std::memory_order_relaxed);
        }
        return x;
    }

    void push(int x) {
        auto b = bottom.load(std::memory_order_relaxed);
        auto t = top.load(std::memory_order_relaxed);
        auto a = array.load(std::memory_order_relaxed);
        if (b - t > a->capacity - 1) { /* Full queue. */
            grow();
            a = array.load(std::memory_order_relaxed);
        }
        a->buffer[b % a->capacity].store(x,std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_release);
        bottom.store(b+1, std::memory_order_relaxed);
    }

    int steal() {
        auto t = top.load(std::memory_order_acquire);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto b = bottom.load(std::memory_order_acquire);
        int x = EMPTY;
        if (t < b) { /* Non-empty queue. */
            auto a = array.load(std::memory_order_consume);  
            x = a->buffer[t % a->capacity].load(std::memory_order_relaxed);
            if (!top.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed)){
                return ABORT; /* Failed race. */
            }
        }
        return x;
    }

private:
    std::atomic<uint64_t>       top; // placed at the mid point 
    std::atomic<uint64_t>       bottom ;
    std::atomic<Array*>         array;
};







}
