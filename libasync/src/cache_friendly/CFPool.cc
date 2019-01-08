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
#include "ConcurrentPrint.h"
#ifdef CFProfiling
#include <iostream>
#include "TimeUtilities.h" // profiling
#endif

#include <exception>

namespace wjp::cf{

std::optional<int> TaskPool::currentThreadIndex()const noexcept
{
    auto me=std::this_thread::get_id();
    for(auto& w : workers){
        if(w.workerThread.get_id()==me) return w.index;
    }
    return {};
}

void TaskPool::start(){
    for(int i=0;i<WorkerNumber;i++){
        workers[i].index=i;
        workers[i].workerThread=std::thread{
            [this,i]{
                if constexpr(VerboseDebug) println("starts!");
                while(!terminating){
                    try{
                        workers[i].routine();
                    }catch(std::exception& e){
                        println("Worker"+std::to_string(i)+e.what());
                    }catch(...){
                        println("Worker"+std::to_string(i)+" unknown exception");
                    }
                }
                if constexpr(VerboseDebug) println("terminated!");
            }
        };
    }
    #ifdef EnableInternalMonitor
    std::thread([this]{
        while(true){
            sleep(InternalMonitorIntervalMS);
            println("\nMonitoring "+std::to_string(WorkerNumber)+" workers:");
            for(auto& w: workers){
                println(w.stat());
            }
        }
    }).detach();
    #endif
}

TaskPool::~TaskPool()
{
    #ifdef CFProfiling
    auto start=wjp::now();
    #endif
    terminating=true;
    #ifdef EnableWorkerSleep
    for(auto& w : workers){
        w.wake();  // notify cv even if it is not blocked
    }
    #endif
    for(auto& w : workers){
        if(w.workerThread.joinable()) w.workerThread.join(); 
    }
    #ifdef CFProfiling
    auto e=wjp::microsec_elapsed_count(start);
    std::cout<<"elapsed="<<e<<"microseconds\n";
    #endif
}



}