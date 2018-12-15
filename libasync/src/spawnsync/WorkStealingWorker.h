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

#include <functional>
#include <memory>
#include <chrono>
#include <optional>
#include <cassert>
#include "ChaseLevDeque.h"
#include "SubmissionBuffer.h"
#include "Task.h"
#include "TimeUtilities.h"

namespace wjp{
class WorkStealingWorkerPool;

// WorkStealingWorker is structured to exploit spatial locality of LLC.
class WorkStealingWorker{
public:
    constexpr static auto idle_timeout=3s;  
    WorkStealingWorker(WorkStealingWorkerPool& pool, int index);
    void routine(); 
    // Should only be called from this worker's owning thread.
    void spawn(std::shared_ptr<Task> task){
        deque->push(task);
    }
    // Could be called from an external non-worker thread. 
    void submit(std::shared_ptr<Task> task){
        buffer->submit(task);
        if(blocked) wake();
    }

    void wake();
protected:
    void becomes_idle(){when_idle_begins=now();} 
    void becomes_busy(){when_idle_begins.reset();}
    // Creates rules to determine if a particular worker will never get blocked.  
    bool immune_to_block() const noexcept;
    // Been idle for too long, it seems meaningless busy wait any more.  
    // Blocking workers should be notified when the worker pool is shut down.
    void try_to_block();
    // Returns if this worker has been idle for more than idle_timeout.
    bool idle_for_too_long();
    // Tries to take a task from local deque first. Only tries to steal from others if 
    // local deque is empty. Returns nullptr if no task could be found after 1 iteration.
    std::shared_ptr<Task> scan_next_task();
    // Starts from this->index, index++ if current worker has nothing left to steal from. 
    // Deterministic stealing pattern exploits temporal locality and hardware prefetching. 
    std::shared_ptr<Task> steal_a_task();
    // Steals from a worker.
    std::shared_ptr<Task> steal_from(WorkStealingWorker&);
private:    
    std::unique_ptr<ChaseLevDeque<Task>> deque;
    std::unique_ptr<SubmissionBuffer<Task>> buffer;
    std::reference_wrapper<WorkStealingWorkerPool> pool;
    std::optional<time_point> when_idle_begins; //Busy if empty, idle otherwise. 
    int index;
    bool blocked=false;
    mutable std::condition_variable cv;
    mutable std::mutex mtx;
};




}