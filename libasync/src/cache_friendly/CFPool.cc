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

#include "CFPool.h"

namespace wjp::cf{

std::optional<int> TaskPool::currentThreadIndex()const noexcept
{
    assert(started);
    auto me=std::this_thread::get_id();
    for(auto& w : workers){
        if(w.workerThread.get_id()==me) return w.index;
    }
    return {};
}

TaskPool::TaskPool()
{
    for(int i=0;i<WorkerNumber;i++){
        workers[i].index=i;
        workers[i].workerThread=std::thread{
            [this,i]{
                while(!started){}
                while(!terminating){
                    workers[i].routine();
                }
            }
        };
    }
    #ifdef EnableInternalMonitor
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

TaskPool::~TaskPool()
{
    terminating=true;
    #ifdef EnableWorkerSleep
    for(auto& w : workers){
        w.wake();  // notify cv even if it is not blocked
    }
    #endif
    for(auto& w : workers){
        w.workerThread.join(); 
    }
}



}