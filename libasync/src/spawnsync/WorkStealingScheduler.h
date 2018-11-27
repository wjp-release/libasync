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

#include <memory>
#include <optional>
#include <vector>
#include <mutex>
#include <condition_variable>

#include "FixedThreadPool.h"
#include "ChaseLevDeque.h"
#include "Callable.h"
#include "ThreadUtilities.h"
#include "WorkStealingRoutine.h"
#include "Task.h"

namespace wjp{

class WorkStealingWorker;
class WorkStealingScheduler{
public:
    // Starts running on creation.
    WorkStealingScheduler();
    // The reference of WorkStealingWorker is returned as a hint to help cancellation.
    std::reference_wrapper<WorkStealingWorker> submit(std::shared_ptr<Task>); 

    template< class R >
    class FuturisticTask : public Task, public Callable<R> {
    public:
        FuturisticTask(WorkStealingScheduler& sched):scheduler(sched)
        {}

        // Note that nullptr_t assignment cannot be inherited, so we need to define it manually. Neither can nullptr_t construtor be inherited, but that makes perfect sense since FuturisticTask must have a scheduler reference ever since its construction. 
        FuturisticTask& operator=(std::nullptr_t)noexcept{
		    Callable<R>::operator=(nullptr);
		    return *this;
	    }
        
        // Pushes this task to work deque or submission buffer
        void submit(){
            std::shared_ptr<Task> task=shared_from_this();
            scheduler().submit(task);
        }

        // Blocks the calling thread until this task finishes
        virtual void wait() override{ 
            std::unique_lock<std::mutex> lk(sync.mtx);
			sync.cv.wait(lk, [this]{return finished;});
        }
        
        // Blocks the calling thread until this task finishes or timeout
        virtual void wait(std::chrono::milliseconds timeout) override{
            std::unique_lock<std::mutex> lk(sync.mtx);
			sync.cv.wait_for(lk, timeout, [this]{return finished;});
        }

        virtual bool is_finished() const noexcept override{
            return finished;
        }

        // A future-like interface to get fulfilled value or rejected reason
        std::optional<R> get(){ 
            wait();
            if(reason){
                 std::rethrow_exception(reason);
            }
            return value; // Could be empty if error happens during execution
        }

        // Does not throw. Get an empty value on failure.
        std::optional<R> get_quietly(){ 
            wait();
            return value; // Could be empty if error happens during execution
        }


        // Set value if the underlying task routine successfully executed. Note that it is not supposed to be called directly by user code. 
        virtual void execute() override{
            try{
                value=this->call();
            }catch(...){
                reason=std::current_exception();
            }
            finished=true;
            sync.cv.notify_all(); 
        }

    private:
        bool finished;
        // Sync is a copy/move constructible & copy/move assignable collection of data members that should not actually be copied or move, e.g., mutex and convars. By doing this trick, the FuturisticTask class can reuse Callable's copy/move constructors and assignment operators.
        struct Sync{ 
            Sync(){} 
            Sync(const Sync& s){} // Do not copy
            Sync(Sync&& s){} // Do not move
            Sync& operator=(Sync&& s){return *this;} // Do not move
            Sync& operator=(const Sync&s){return *this;} // Do not copy
            mutable std::mutex mtx; 
            mutable std::condition_variable cv;  // Notified on finished
        };
        mutable Sync sync; 
        std::optional<R> value; // Default: nullopt
        std::exception_ptr reason; // Default: nullptr
        std::reference_wrapper<WorkStealingScheduler> scheduler;
    };

    template < class R >
    std::shared_ptr<FuturisticTask<R>> create_futuristic_task(){
        return std::make_shared<FuturisticTask<R>>(*this);
    }


private:
    std::unique_ptr<FixedThreadPool<WorkStealingWorker>> pool;
};


}