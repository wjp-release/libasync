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

// Kicks it off on construction
WorkStealingScheduler::WorkStealingScheduler() : 
        pool(std::make_unique<WorkStealingWorkerPool>(recommended_nr_thread()))
{
    pool->start();
}

// If called from a worker thread, push the task to its deque, 
// otherwise, to a randomly picked worker's submission buffer.
std::reference_wrapper<WorkStealingWorker> WorkStealingScheduler::sched(std::shared_ptr<Task> task)
{
    auto worker=pool->current_thread_handle();
    if(worker){ 
        WorkStealingWorker& w=worker.value().get();
        w.spawn(task);
        return worker.value();
    }else{ 
        std::reference_wrapper<WorkStealingWorker> submission_target=pool->randomly_pick_one();
        WorkStealingWorker& w=submission_target.get();
        w.submit(task);
        return submission_target;
    }
}

std::reference_wrapper<WorkStealingWorker> WorkStealingScheduler::get_worker()
{
    auto worker=pool->current_thread_handle();
    if(worker){ 
        return worker.value();
    }else{ 
        assert(false); //impossible! get_worker must be called from a worker thread!
    }
}


std::reference_wrapper<WorkStealingWorker> WorkStealingScheduler::which_worker()
{
    auto worker=pool->current_thread_handle();
    if(worker){ 
        return worker.value();
    }else{ 
        return pool->randomly_pick_one();
    }
}

bool WorkStealingScheduler::is_called_from_external()  
{
    auto worker=pool->current_thread_handle();
    return !worker.has_value();
}


}