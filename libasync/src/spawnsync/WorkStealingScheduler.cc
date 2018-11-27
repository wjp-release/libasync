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

#include "WorkStealingScheduler.h"
#include "WorkStealingWorker.h"
#include <iostream>

namespace wjp{

WorkStealingScheduler::WorkStealingScheduler() 
{
    std::vector<WorkStealingWorker> workers{};
    auto nr_threads=recommended_nr_thread();
    //auto nr_threads=2;

    for(int i=0;i<nr_threads;i++){
        workers.emplace_back(*this);
    }
    pool=std::make_unique<FixedThreadPool<WorkStealingWorker>>(workers, WorkStealingRoutine::thread_func); 
    pool->start();
}

std::reference_wrapper<WorkStealingWorker> WorkStealingScheduler::submit(std::shared_ptr<Task> task)
{
    auto worker=pool->current_thread_handle();
    if(worker){ //Called from a worker thread, push to its deque
        WorkStealingWorker& w=worker.value().get();
        w.push_to_deque(task);
        return worker.value();
    }else{ //Called from an external thread, push to a randomly picked worker's submission buffer
        std::reference_wrapper<WorkStealingWorker> submission_target=pool->randomly_pick_one();
        WorkStealingWorker& w=submission_target.get();
        w.push_to_buffer(task);
        w.push_to_deque(task);  // delete this line, test only!
        return submission_target;
    }
}



}