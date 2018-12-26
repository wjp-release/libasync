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

#include "WorkStealingWorkerPool.h"
#include <cassert>
#include "ThreadUtilities.h"
#include "ConcurrentPrint.h"

namespace wjp {

WorkStealingWorkerPool::WorkStealingWorkerPool(int nr_workers) :
    threads(nr_workers), workers(nr_workers)
{
    for(int i=0; i<nr_workers; i++){
        workers.emplace_back(*this, i);
    }
    for(int i=0; i<nr_workers; i++){
        threads.emplace_back([this,i]{
            if(!started){}
            while(!terminating){
                workers[i].routine(); 
            }
        });
    }
    #ifdef SAMPLE_DEBUG
    // Internal Monitor Thread
    std::thread([this, nr_workers]{
        while(true){
            sleep(1000);
            println("\nMonitoring "+std::to_string(nr_workers)+" worker:");
            for(auto& w: workers){
                println(w.stat());
            }
        }
    }).detach();
    #endif
}

WorkStealingWorkerPool::~WorkStealingWorkerPool() 
{
    terminating=true;  
    for(auto& w : workers){
        w.wake();  // notify cv even if it is not blocked
    }
    for(auto& t : threads){
        t.join(); 
    }
}

std::reference_wrapper<WorkStealingWorker> WorkStealingWorkerPool::get_worker(int index)
{
    assert(index>=0&&index<nr_threads());
    return std::ref(workers[index]);
}

std::optional<int> WorkStealingWorkerPool::current_thread_index()const noexcept
{
    assert(started);
    auto me=std::this_thread::get_id();
    for(int i=0;i<threads.size();i++){
        if(threads[i].get_id()==me)return i;
    }
    return {};
}

std::optional<std::reference_wrapper<WorkStealingWorker>> WorkStealingWorkerPool::current_thread_handle()noexcept{
    assert(started);
    auto index=current_thread_index();
    if(index){
        return std::ref(workers[index.value()]);
    }else{
        return {};
    }
}

std::reference_wrapper<WorkStealingWorker> WorkStealingWorkerPool::randomly_pick_one()noexcept
{
    assert(started);
    int index=randinteger(0, threads.size()-1);
    return std::ref(workers[index]);
}


void WorkStealingWorkerPool::wake_all_sleeping_workers()
{
    for(auto& w : workers){
        w.wake_if_blocked();
    }
}


}

