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
#include <shared_mutex>
#include <condition_variable>
#include <atomic>

#include "WorkStealingWorkerPool.h"
#include "ChaseLevDeque.h"
#include "Callable.h"
#include "ThreadUtilities.h"
#include "Task.h"
#include "Config.h"

namespace wjp{

//#define INTERNAL_STATS

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
#ifdef INTERNAL_STATS
    std::atomic<int> nr_tasks_executed=0;  
#endif 
    int nr_workers()const noexcept{
        return pool->nr_threads();
    }
    // Starts running on creation.
    WorkStealingScheduler();
    template< class R >
    class FuturisticTask : public Task, public Callable<R> {
    public:
        enum TaskState: int {
            freelance=0,  // in_pool --> cancel --> freelance
            in_pool=1,
            sched=2, // in_pool --> taken/stolen from deque/buffer --> sched
            done=3,
        };
        FuturisticTask(WorkStealingScheduler& sched):scheduler(sched)
        {}
        // Note that nullptr_t assignment cannot be inherited, so we need to define it manually. Neither can nullptr_t construtor be inherited, but that makes perfect sense since FuturisticTask must have a scheduler reference ever since its construction. 
        FuturisticTask& operator=(std::nullptr_t)noexcept{
		    Callable<R>::operator=(nullptr);
		    return *this;
	    }
        // Pushes this task to work deque or submission buffer
        virtual void submit() override{
            assert(state==freelance);
            ready_for_sched(); // set state to in_pool! 
            std::shared_ptr<Task> task=shared_from_this();
            scheduler.get().sched(task);
        }
        virtual bool is_waitable(){
            int s=state;
            return (s==in_pool||s==sched);
        }
        // Blocks the calling thread until this task finishes. It doesn't wait for freelance or done tasks.
        virtual void wait() override{ 
            if(is_waitable()){
                std::unique_lock<std::mutex> lk(Task::sync.mtx);
                Task::sync.cv.wait(lk, [this]{return is_finished();});
            } 
        }
        // Blocks the calling thread until this task finishes or timeout
        virtual void wait(std::chrono::milliseconds timeout) override{
            if(is_waitable()){
                std::unique_lock<std::mutex> lk(Task::sync.mtx);
                Task::sync.cv.wait_for(lk, timeout, [this]{return is_finished();});
            }
        }
        // Lazy cancel: simpley remark a task as freelance. 
        // Its container should ignore freelance tasks in their take()/steal() operations. 
        virtual bool cancel() override{
            #ifdef WITH_CANCEL
            std::unique_lock lk(Task::sync.cancel_mtx); //write lock
            if(state==in_pool){
                state=freelance; 
                return true;
            }
            #endif
            return false; // damn too late
        }
        // You might want to recycle a task and resubmit it without creating a new task.
        void recycle(){
            state=freelance;
            reason=nullptr;
            value.reset();
        }
        // [Probably not often used] You may need this function if you want to submit this task to your custom schedulers. You must set state to in_pool before executing the task asynchronously.
        void ready_for_sched(){
            state=in_pool;
        }
        // Called by worker thread when it takes/steals a task
        virtual void to_sched() override{
            state=sched;
        }
        virtual bool is_finished() const noexcept override{
            return state==done;
        }
        virtual bool is_canceled() const noexcept override{
            return state==freelance;
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
            state=done;
            Task::sync.cv.notify_all(); 
        }
        // Freelance execution without changing its state (it might still lie in a pool with canceled state, which we cannot modify)
        virtual void compute() override{
            try{
                value=this->call();
            }catch(...){
                reason=std::current_exception();
            }
        }
    protected:
        int state = freelance;
        std::optional<R> value; // Default: nullopt
        std::exception_ptr reason; // Default: nullptr
        std::reference_wrapper<WorkStealingScheduler> scheduler;
    };

    template < class R >
    std::shared_ptr<FuturisticTask<R>> create_futuristic_task(){
        #ifdef INTERNAL_STATS
        nr_tasks_executed++; 
        #endif 
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
        // Worker threads are not supposed to stall by nature. If called from worker thread, wait() will steal back tasks from its stealers to avoid deadlock and accelerate the completion of this task's child tasks. 
        // It only blocks when it is called from an external thread.
        virtual void wait() override{ 
            WorkStealingScheduler& scheduler=FuturisticTask<R>::scheduler.get();
            if(scheduler.is_called_from_external()){
                FuturisticTask<R>::wait();
            }else{ 
                // Without Cancel&Exec: (162+256+170+56+225+60+76+110+45+48+198+177)/12=132
                #ifdef WITH_CANCEL
                if(FuturisticTask<R>::cancel()){  // cancel & execute locally if possible
                    FuturisticTask<R>::compute();
                    return;
                }
                #endif
                WorkStealingWorker& worker=scheduler.get_worker().get();
                while(!FuturisticTask<R>::is_finished()){ 
                    worker.join_routine();  
                }
            }
        }
        virtual void wait(std::chrono::milliseconds timeout) override{
            WorkStealingScheduler& scheduler=FuturisticTask<R>::scheduler.get();
            if(scheduler.is_called_from_external()){
                FuturisticTask<R>::wait(timeout);
            }else{ 
                time_point start=now();
                WorkStealingWorker& worker=scheduler.get_worker().get();
                while(!FuturisticTask<R>::is_finished()){ 
                    worker.join_routine();  
                    if(ms_elapsed(start)>timeout) break;
                }
            }
        }
        // [You don't need it with pipable semantics] Forces the underlying callable to take WorkStealingScheduler& as its 1st argument. 
        template< class F, class... Args, class=std::enable_if_t<(std::is_invocable<F, WorkStealingScheduler&, Args...>{})>>
        void scheduler_aware_bind(F&& f, Args&&... args ){
            Callable<R>::bind(std::forward<F>(f), FuturisticTask<R>::scheduler, std::forward<Args>(args)...);
        }
        // ForkJoinTask's alias for submit(); exactly the same.
        void fork(){
            FuturisticTask<R>::submit();
        }
        // ForkJoinTask's alias for get_quietly(); underlying wait() is different though.
        std::optional<R> join_quietly(){ 
            return FuturisticTask<R>::get_quietly();
        }
        // ForkJoinTask's alias for get(); underlying wait() is different though.
        R join(){ 
            return FuturisticTask<R>::get();
        }
    };

    // Note that this function should usually be called in worker thread. 
    template < class R >
    std::shared_ptr<ForkJoinTask<R>> create_forkjoin_task(){
        #ifdef INTERNAL_STATS
        nr_tasks_executed++; 
        #endif 
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