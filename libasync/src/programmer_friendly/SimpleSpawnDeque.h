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

#include <list>
#include <mutex>
#include <algorithm>
#include <memory>
#include <shared_mutex>
#include "Config.h"
namespace wjp{

template <class T>
class SimpleSpawnDeque
{
public:
    // Pushes a task to back.
    void push(std::shared_ptr<T> task){
        std::lock_guard<std::mutex> lk(mtx);
        task_list.push_back(task);
    }
    // Tries to remove a task, usually called when caller wants to run the task locally.
    bool withdraw(std::shared_ptr<T> task){
        std::lock_guard<std::mutex> lk(mtx); 
        auto it=std::find(task_list.begin(), task_list.end(), task);
        if(it==task_list.end()) return false;
        task_list.erase(it);
        return true;
    }

    // Ignores canceled tasks.                 
    std::shared_ptr<T> take_ignore_canceled() {
        #ifdef WITH_CANCEL
        std::shared_ptr<T> task=take();
        std::shared_ptr<T> next=nullptr;
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
    std::shared_ptr<T> steal_ignore_canceled() {
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
    // Tries to steal an oldest task at front.
    std::shared_ptr<T> steal(){
        std::lock_guard<std::mutex> lk(mtx); 
        if(task_list.empty()) return nullptr;
        auto task_ptr=task_list.front();
        task_list.pop_front();
        return task_ptr;
    }
    // Tries to take a newest task at back.
    std::shared_ptr<T> take(){
        std::lock_guard<std::mutex> lk(mtx); 
        if(task_list.empty()) return nullptr;
        auto task_ptr=task_list.back();
        task_list.pop_back();
        return task_ptr;
    }
    // Inefficient; should only be used for debugging/monitoring.
    uint64_t size(){
        return (uint64_t)task_list.size();
    }
private:
    std::list<std::shared_ptr<T>> task_list;
    mutable std::mutex mtx;
};

}