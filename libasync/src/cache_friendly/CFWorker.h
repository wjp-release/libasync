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

#include "CFDeque.h"
#include "CFBuffer.h"
#include <optional>
#include <mutex>
#include <condition_variable>
#include "TimeUtilities.h"
#include "CFScheduler.h"

namespace wjp::cf{

class Worker{
public:
    constexpr static auto idle_timeout=3s;  
    // Worker(TaskScheduler&, int index);
    // void routine(); 

    template < class T, class... Args >  
    T*                          emplaceTaskInDeque(Args&&... args){
        return deque.emplaceTask<T>(std::forward<Args>(args)...);
    }

    template < class T, class... Args >  
    T*                          emplaceTaskInBuffer(Args&&... args){
        return buffer.emplaceTask<T>(std::forward<Args>(args)...);
    }

    // void submit(Task* task);
    // std::string stat(); // Debugging/monitoring-only stats.
    // void wake();
    // void wake_if_blocked();
    // void join_routine();
protected:
    // void simple_frogleaping(); 
    // bool is_idle()noexcept{return when_idle_begins.has_value();}
    // void becomes_idle(){when_idle_begins=now();} 
    // void becomes_busy(){when_idle_begins.reset();}
    // bool immune_to_block() const noexcept;
    // void try_to_block();
    // bool idle_for_too_long();
    // Task* scan_next_task();
    // Task* steal_a_task();
    // Task* steal_from(WorkStealingWorker&);
private:
    TaskDeque                           deque;
    TaskBuffer                          buffer;
    TaskScheduler                       scheduler;
    std::optional<time_point>           idleBeginTime; //Busy if empty, idle otherwise. 
    int                                 index;
    volatile int                        stealerIndex=-1; // hint for steal-back algorithm
    bool                                blocked=false;
    mutable std::condition_variable     cv; 
    mutable std::mutex                  mtx;
};


}
