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

#include <optional>
#include <cassert>
#include <atomic>

#include "CFConfig.h"
#include "CFWorker.h"
#include "RandomUtilities.h"
#include "ThreadUtilities.h"
#include "ConcurrentPrint.h"

namespace wjp::cf{

class TaskPool{
public:
	TaskPool(const TaskPool&) = delete;
    void operator=(const TaskPool&) = delete;
	static TaskPool&    instance(){
		static TaskPool pool; 
		return pool;
	}
    Worker&             getWorker(int index) noexcept{
        if constexpr(EnableAssert) assert(index>=0&&index<WorkerNumber);
        return workers[index];    
    }
    Worker&             randomlyPickOne() noexcept{
        return workers[randinteger(0, WorkerNumber-1)];
    }
    void                terminate() noexcept{
        terminating=true;
    }

    void                start();
    std::optional<int>  currentThreadIndex()const noexcept;

    template < class T, class... Args >  
    T*                  emplace(Args&&... args)
    {
        auto current_worker=currentThreadIndex();
        if(current_worker.has_value()){
            int index=current_worker.value();
            return workers[index].deque.emplace<T>(std::forward<Args>(args)...);
        }else{
            Worker& w=randomlyPickOne();
            return w.buffer.emplace<T>(std::forward<Args>(args)...);
        }
    }

    template < class T, class... Args >  
    T*                  emplaceExternally(Args&&... args)
    {
        Worker& w=randomlyPickOne();
        return w.buffer.emplace<T>(std::forward<Args>(args)...);
    }

    template < class T, class... Args >  
    T*                  emplaceLocally(Args&&... args)
    {
        auto current_worker=currentThreadIndex();
        if constexpr(EnableAssert) assert(current_worker.has_value() && "This method can only be called by a worker thread.");
        int index=current_worker.value();
        return workers[index].deque.emplace<T>(std::forward<Args>(args)...);
    }

    // Can only be called by a worker thread.
    template < class T, class... Args >  
    T*                  emplaceAsExec(Args&&... args)
    {
        auto current_worker=currentThreadIndex();
        if constexpr(EnableAssert) assert(current_worker.has_value() && "This method can only be called by a worker thread.");
        int index=current_worker.value();
        return workers[index].deque.emplaceAsExec<T>(std::forward<Args>(args)...);
    }

    #ifdef EnableWorkerSleep
    void                wakeAllSleepingWorkers()noexcept
    {
        for(auto& w : workers){
            w.wakeIfBlocked();
        }
    }
    #endif
protected:
	TaskPool() noexcept :terminating(false){
        for(uint8_t i=0;i<WorkerNumber;i++){
            workers[i].setIndex(i);
        }
    }
    ~TaskPool();
private:
    bool                isCalledByWorkerThread()noexcept{
        return currentThreadIndex().has_value();
    }
    volatile bool       terminating;  
	//volatile std::atomic<bool>       terminating;  
    Worker              workers[WorkerNumber];
};


}