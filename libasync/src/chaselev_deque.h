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
#include <memory>
#include <iostream>

//optional 

// take & push are 
// steal could be invoked by any thread
// see https://www.dre.vanderbilt.edu/~schmidt/PDF/work-stealing-dequeue.pdf
// and https://www.di.ens.fr/~zappa/readings/ppopp13.pdf

namespace wjp{

// T is the type of a task
// Tasks are stored in std::shared_ptr<T>  
template<typename T>
class chaselev_deque
{
public:
    // initial capacity must be power of 2
    chaselev_deque(int log2capacity=13)  : array(new atomic_array(1<<log2capacity)),
                                top(1<<(log2capacity-1)), bottom(1<<(log2capacity-1))
    {
        print();
    }

    // push, grow, take could only be invoked by the deque's owning thread
    struct atomic_array{
        atomic_array(uint64_t cap) : buffer(new std::shared_ptr<T>[1<<cap]), capacity(cap) {}
        ~atomic_array(){delete[]buffer;}
        std::shared_ptr<T>*     buffer;     // should be replaced by std::atomic<std::shared_ptr> in C++20
        std::atomic<uint64_t>   capacity;   // power of 2, grow at rate of *2 each time, init: 2^13(8K), max: 2^26(64M)
        std::shared_ptr<T>&     at(uint64_t index)
        {
            return buffer[index%capacity];
        }
    };

    struct autorelease{  
        autorelease(atomic_array*arr):arr(arr){}
        atomic_array* arr;
        ~autorelease(){
            delete arr;
        }
    };

    void print(){
        auto arr = array.load(std::memory_order_relaxed);
        std::cout<<"size="<<arr->capacity.load()<<", top="<<top.load()<<", bottom="<<bottom.load()<<std::endl;
        std::cout<<"[top ... bottom]: ";
        for(uint64_t i=top.load(std::memory_order_relaxed);i<bottom.load(std::memory_order_relaxed);i++){
            std::cout<<arr->at(i)<<" ";
        }  
        std::cout<<"| buffer="<< arr->buffer<<std::endl;
        std::cout<<std::endl;
    }

    void grow(){
        auto old_arr = array.load(std::memory_order_relaxed);
        uint64_t new_capacity=old_arr->capacity<<1;
        autorelease guard{old_arr}; //must declare variable name here, otherwise old_arr would be destroyed early
        auto new_arr=new atomic_array(new_capacity);
        for(uint64_t i=top.load(std::memory_order_relaxed);i<bottom.load(std::memory_order_relaxed);i++)  
        {
            auto x = std::atomic_load_explicit(&old_arr->at(i), std::memory_order_relaxed);
            std::atomic_store_explicit(&new_arr->at(i), x, std::memory_order_relaxed);
        }
        array.store(new_arr, std::memory_order_relaxed);
    }

    std::shared_ptr<T> take() {
        auto b=bottom.load(std::memory_order_relaxed)-1;
        auto a=array.load(std::memory_order_relaxed);
        bottom.store(b,std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto t=top.load(std::memory_order_relaxed);
        std::shared_ptr<T> x;
        if (t <= b) { /* Non-empty queue. */
            x = std::atomic_load_explicit(&a->at(b), std::memory_order_relaxed);
            if (t == b) { /* Single last element in queue. */
                if(!top.compare_exchange_strong(t,t+1,std::memory_order_seq_cst, std::memory_order_relaxed)){
                    x=nullptr;
                }
                bottom.store(b+1, std::memory_order_relaxed);
            }
        } else { /* Empty queue. */
            x=nullptr;
            bottom.store(b+1, std::memory_order_relaxed);
        }
        return x;
    }

    void push(std::shared_ptr<T> x) {
        auto b = bottom.load(std::memory_order_relaxed);
        auto t = top.load(std::memory_order_relaxed);
        auto a = array.load(std::memory_order_relaxed);
        if (b - t > a->capacity - 1) { /* Full queue. */
            grow();
            a = array.load(std::memory_order_relaxed);
        }
        std::atomic_store_explicit(&a->at(b), x, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_release);
        bottom.store(b+1, std::memory_order_relaxed);
    }

    std::shared_ptr<T> steal() {
        auto t = top.load(std::memory_order_acquire);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto b = bottom.load(std::memory_order_acquire);
        std::shared_ptr<T> x = nullptr;
        if (t < b) { 
            auto a = array.load(std::memory_order_consume);  
            x = std::atomic_load_explicit(&a->at(t), std::memory_order_relaxed);
            if (!top.compare_exchange_strong(t, t + 1, std::memory_order_seq_cst, std::memory_order_relaxed)){
                return nullptr; 
            }
        }
        return x;
    }

private:
    std::atomic<uint64_t>       top;
    std::atomic<uint64_t>       bottom ;
    std::atomic<atomic_array*>  array;
};







}
