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
#include "CFConfig.h"
#include "CFDeque.h"
#include "CFBuffer.h"
#include <optional>
#include <mutex>
#include <condition_variable>
#include "TimeUtilities.h"
#include <thread>

namespace wjp::cf{
class TaskScheduler;
class Worker{
public:
    friend class TaskPool;
    friend class Task;
    // obsolete; 
    template < class T, class... Args >  
    T*                              emplaceTaskInDeque(Args&&... args){
        return deque.emplace<T>(std::forward<Args>(args)...);
    }
    // obsolete; 
    template < class T, class... Args >  
    T*                              emplaceTaskInBuffer(Args&&... args){
        return buffer.emplace<T>(std::forward<Args>(args)...);
    }

    #ifdef EnableWorkerSleep
    void                            wake(){

    }
    void                            wakeIfBlocked(){

    }
    #endif
void                            reclaim(Task* executed) noexcept;
    void                            routine(); 
    std::string                     stat(); 
    void                            setIndex(uint8_t workerIndex) noexcept{
        index=workerIndex;
        deque.setIndex(index);
        buffer.setIndex(index);
    }
    // void join_routine();
protected:
    Task*                           stealFromBuffer();
    Task*                           stealFromBufferOf(Worker& worker);
    Task*                           stealFromDeque();
    Task*                           stealFromDequeOf(Worker& worker);
    // void simple_frogleaping(); 
    // bool is_idle()noexcept{return when_idle_begins.has_value();}
    // void becomes_idle(){when_idle_begins=now();} 
    // void becomes_busy(){when_idle_begins.reset();}
    // bool immune_to_block() const noexcept;
    // void try_to_block();
    // bool idle_for_too_long();
    // Task* scan_next_task();
private:
    TaskDeque                       deque;
    TaskBuffer                      buffer;
    uint8_t                         index=-1; // inited by TaskPool's constructor
    // volatile uint8_t                stealerIndex=-1; // hint for steal-back algorithm
    std::thread                     workerThread;
    #ifdef EnableWorkerSleep
    std::optional<time_point>       idleBeginTime; //Busy if empty, idle otherwise. 
    bool                            blocked=false;
    mutable std::condition_variable cv; 
    mutable std::mutex              mtx;
    #endif

};


}
