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

#include "WorkStealingWorkerPool.h"
#include "ChaseLevDeque.h"
#include "Callable.h"
#include "ThreadUtilities.h"
#include "Task.h"

namespace wjp{

class WorkStealingWorker;
class WorkStealingScheduler{
protected:
    // The reference of WorkStealingWorker is returned as a hint to help cancellation.
    std::reference_wrapper<WorkStealingWorker> sched(std::shared_ptr<Task>); 
    // Returns owning worker if called from worker thread. A randomly picked worker otherwise.
    std::reference_wrapper<WorkStealingWorker> which_worker();
    // Must be called from a worker thread.
    std::reference_wrapper<WorkStealingWorker> get_worker();

    bool is_called_from_external();
public:
    int nr_workers()const noexcept{
        return pool->nr_threads();
    }
    // Deprecated. (Reserved for test compatibility) 
    std::reference_wrapper<WorkStealingWorker> submit(std::shared_ptr<Task> task){
        return sched(task);
    }
    // Starts running on creation.
    WorkStealingScheduler();
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
        virtual void submit(){
            std::shared_ptr<Task> task=shared_from_this();
            scheduler.get().sched(task);
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
        R get(){ 
            wait();
            if(reason){
                 std::rethrow_exception(reason);
            }
            assert(value.has_value());
            return value.value(); // Could be empty if error happens during execution
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

    protected:
        bool finished=false; 
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

    // Fork/Join extensions of FuturisticTask
    template< class R >
    class ForkJoinTask : public FuturisticTask<R> {
    public:
        ForkJoinTask(WorkStealingScheduler& sched) : FuturisticTask<R>(sched)
        {}

        ForkJoinTask& operator=(std::nullptr_t)noexcept{
		    FuturisticTask<R>::operator=(nullptr);
		    return *this;
	    }
        // Quiet version of join. It does not throw. You can tell it's failed if optional<R> is empty.
        std::optional<R> join_quietly(){ 
            WorkStealingScheduler& scheduler=FuturisticTask<R>::scheduler.get();
            // External join() behaves exactly like get() except that the task must be scheduler-aware binded
            if(scheduler.is_called_from_external()){
                FuturisticTask<R>::wait();
            }else{  // worker thread's join() must be lock-free! 
                // we can't sit here and wait, try to do some work, like stealing back ... 
                // otherwise the concurrency of this thread will be lost, 
                // the entire system will easily fall into deadlock as well.
                WorkStealingWorker& worker=scheduler.get_worker().get();
                while(!FuturisticTask<R>::finished){ 
                    worker.join_routine(); // helping other workers finish 
                }
            }
            return FuturisticTask<R>::value; // Could be empty if error happens during execution
        }


        // Note that worker threads are not supposed to stall by nature. If called from worker thread, join() will steal back tasks from its stealers to avoid deadlock and accelerate the completion of this task's child tasks. 
        R join(){ 
            WorkStealingScheduler& scheduler=FuturisticTask<R>::scheduler.get();
            // External join() behaves exactly like get() except that the task must be scheduler-aware binded
            if(scheduler.is_called_from_external()){
                FuturisticTask<R>::wait();
            }else{  // worker thread's join() must be lock-free! 
                // we can't sit here and wait, try to do some work, like stealing back ... 
                // otherwise the concurrency of this thread will be lost, 
                // the entire system will easily fall into deadlock as well.
                WorkStealingWorker& worker=scheduler.get_worker().get();
                while(!FuturisticTask<R>::finished){ 
                    worker.join_routine(); // helping other workers finish 
                }
            }
            if(FuturisticTask<R>::reason){
                std::rethrow_exception(FuturisticTask<R>::reason);
            }
            assert(FuturisticTask<R>::value.has_value());
            return FuturisticTask<R>::value.value(); // Could be empty if error happens during execution
        }

        // ForkJoinTask callable now tasks the reference to the WorkStealingScheduler as its 1st argument.
        template< class F, class... Args, class=std::enable_if_t<(std::is_invocable<F, WorkStealingScheduler&, Args...>{})>>
        void scheduler_aware_bind(F&& f, Args&&... args ){
            // The type of FuturisticTask<R>::scheduler is std::reference_wapper<WorkStealingScheduler>,
            // but the first argument of F should be WorkStealingScheduler&.  
            Callable<R>::bind(std::forward<F>(f), FuturisticTask<R>::scheduler, std::forward<Args>(args)...);
        }

        void fork(){
            FuturisticTask<R>::submit();
        }
    };

    // Note that this function should usually be called in worker thread. 
    template < class R >
    std::shared_ptr<ForkJoinTask<R>> create_forkjoin_task(){
        return std::make_shared<ForkJoinTask<R>>(*this);
    }

    template< class R, class F, class... Args, class=std::enable_if_t<(std::is_invocable<F, WorkStealingScheduler&, Args...>{})>>
    std::shared_ptr<ForkJoinTask<R>> spawn(F&& f, Args&&... args){
        auto subtask=create_forkjoin_task<R>();
        subtask->scheduler_aware_bind(std::forward<F>(f),std::forward<Args>(args)...);
        subtask->fork(); 
        return subtask;
    }

private:
    std::unique_ptr<WorkStealingWorkerPool> pool;
};



}