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

#include "FixedThreadPool.h"
#include "ChaseLevDeque.h"

namespace wjp::WorkStealing{

class Scheduler;

// Thread-local data handle
class Worker{
public:
    Worker(Scheduler& scheduler);

private:
    Scheduler& scheduler;
};

extern void thread_func(FixedThreadPool& pool, Worker& worker);

class Scheduler{
public:
    // Starts running on creation
    Scheduler() 
    {
        auto nr_threads=FixedThreadPool::recommended_nr_thread();
        for(int i=0;i<nr_threads;i++){
            workers.emplace_back(*this);
        }
        //pool=std::make_unique<FixedThreadPool>(std::ref(workers), thread_func); 
    }

private:
    std::unique_ptr<FixedThreadPool> pool;
    std::vector<Worker> workers{};
};


}