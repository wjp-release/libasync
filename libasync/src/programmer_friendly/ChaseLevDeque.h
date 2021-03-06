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

#include <memory>
#include <atomic>
#include <iostream>
#include "AutoRelease.h"
#include <mutex>
#include <shared_mutex>
#include "Config.h"
namespace wjp{

// An optimized version of Chase-Lev Work Stealing Deque.

// see https://www.dre.vanderbilt.edu/~schmidt/PDF/work-stealing-dequeue.pdf
// and https://www.di.ens.fr/~zappa/readings/ppopp13.pdf

// Tasks are stored in std::shared_ptr<T> in ChaseLevDeque<T>
template <class T>
class ChaseLevDeque
{
public:
    // ChaseLevDeque takes log2(capacity) as argument to guarantee that its capacity is power of 2. 
    ChaseLevDeque(int log2capacity=10)  :   top((uint64_t)1<<(log2capacity-1)), 
                                            bottom((uint64_t)1<<(log2capacity-1)),
                                            array(new SharedPointerArray((uint64_t)1<<log2capacity))
    {}
    struct SharedPointerArray{
        //Uses placement new to create shared_ptr in given buffer.
        SharedPointerArray(uint64_t cap) :
            buffer(reinterpret_cast<std::shared_ptr<T>*>(new char[sizeof(std::shared_ptr<T>)*cap])), 
            capacity(cap) {
            for(uint64_t i=0;i<capacity;i++){ 
                new(reinterpret_cast<char*>(&buffer[i])) std::shared_ptr<T>{nullptr}; 
            }
        }
        ~SharedPointerArray(){
            for(uint64_t i=0;i<capacity;i++){
                buffer[i].~shared_ptr(); 
            }
            delete [] reinterpret_cast<char*>(buffer);
        }
        std::shared_ptr<T>*     buffer;   
        std::atomic<uint64_t>   capacity;     
        std::shared_ptr<T>&     at(uint64_t index){
            return buffer[index%capacity];
        }
    };
    // Shows some internal stats for debugging.
    void                                print(){
        auto arr = array.load(std::memory_order_relaxed);
        std::cout<<"size="<<arr->capacity.load()<<", top="<<top.load()<<", bottom="<<bottom.load()<<std::endl;
        std::cout<<"[top ... bottom]: ";
        for(uint64_t i=top.load(std::memory_order_relaxed);i<bottom.load(std::memory_order_relaxed);i++){
            std::cout<<arr->at(i)<<" ";
        }  
        std::cout<<"| buffer="<< arr->buffer<<std::endl;
        std::cout<<std::endl;
    }
    // Utterly unsafe; should only be used for debugging/monitoring.
    uint64_t                            size() noexcept{
        return bottom.load()-top.load();
    }        
    // Ignores canceled tasks.                 
    std::shared_ptr<T>					take_ignore_canceled() {
        #ifdef WITH_CANCEL
        std::shared_ptr<T> task=take(); // cannot change what task points to before lock gets released,
        std::shared_ptr<T> next=nullptr; // so we need another task_ptr that points to newly taken task 
        while(task!=nullptr){
            {
                std::shared_lock lk(task->sync.cancel_mtx); 
                if(task->is_canceled()){
                    next=take();
                }else{
                    task->to_sched();
                    break;
                }
            } // read lock must be released before switching to another task
            //original task will be destroyed here if ref count=1
            //lk must unlock before its owning task's destruction
            task=next;
        }
        return task;
        #else
        std::shared_ptr<T> task=take(); 
        if(task==nullptr) return nullptr;
        task->to_sched();
        return task;
        #endif
    }
    // Ignores canceled tasks.                 
    std::shared_ptr<T>					steal_ignore_canceled() {
        #ifdef WITH_CANCEL
        std::shared_ptr<T> task=steal();
        std::shared_ptr<T> next=nullptr;
        while(task!=nullptr){
            {
                std::shared_lock lk(task->sync.cancel_mtx);
                if(task->is_canceled()){
                    next=steal();
                }else{
                    task->to_sched();
                    break;
                }
            }
            task=next;
        }
        return task;
        #else
        std::shared_ptr<T> task=steal(); 
        if(task==nullptr) return nullptr;
        task->to_sched();
        return task;
        #endif
    }
    // Tries to take a recently pushed task. It should only be called from the owning thread.
    std::shared_ptr<T>					take() {
        auto b=bottom.load(std::memory_order_relaxed)-1;
        auto a=array.load(std::memory_order_relaxed);
        bottom.store(b,std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_seq_cst);
        auto t=top.load(std::memory_order_relaxed);
        std::shared_ptr<T> x;
        if (t <= b) { 
            x = std::atomic_load_explicit(&a->at(b), std::memory_order_relaxed);
            if (t == b) { 
                if(!top.compare_exchange_strong(t,t+1,std::memory_order_seq_cst, std::memory_order_relaxed)){
                    x=nullptr;
                }
                bottom.store(b+1, std::memory_order_relaxed);
            }
        } else { 
            x=nullptr;
            bottom.store(b+1, std::memory_order_relaxed);
        }
        return x;
    }
    // Pushes a task to bottom. It should only be called from the owning thread.
    void                                push(std::shared_ptr<T> x) {
        auto b = bottom.load(std::memory_order_relaxed);
        auto t = top.load(std::memory_order_relaxed);
        auto a = array.load(std::memory_order_relaxed);
        if (b - t > a->capacity - 1) { // full
            grow();
            a = array.load(std::memory_order_relaxed);
        }
        std::atomic_store_explicit(&a->at(b), x, std::memory_order_relaxed);
        std::atomic_thread_fence(std::memory_order_release);
        bottom.store(b+1, std::memory_order_relaxed);
    }
    // Tries to steal the oldest task. It can be called from any thread, usually a stealer.
    std::shared_ptr<T>                  steal() {
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
protected:
    // Doubles the underlying array's size. It is called when the deque becomes full.
    void                                grow(){
        auto old_arr = array.load(std::memory_order_relaxed);
        uint64_t new_capacity=old_arr->capacity<<1;
        AutoDelete<SharedPointerArray> delguard{old_arr}; 
        auto new_arr=new SharedPointerArray(new_capacity);
        for(uint64_t i=top.load(std::memory_order_relaxed);i<bottom.load(std::memory_order_relaxed);i++)  
        {
            auto x = std::atomic_load_explicit(&old_arr->at(i), std::memory_order_relaxed);
            std::atomic_store_explicit(&new_arr->at(i), x, std::memory_order_relaxed);
        }
        array.store(new_arr, std::memory_order_relaxed);
    }
private:
    std::atomic<uint64_t>               top;
    std::atomic<uint64_t>               bottom;
    std::atomic<SharedPointerArray*>    array;
};

}