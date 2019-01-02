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

#include "WorkStealingScheduler.h"
#include <boost/hof.hpp>
#include <string>
#include "Config.h"

namespace wjp{

// Note that: to use pipable async semantics,
// you must register a single global instance of scheduler.
// If you have multiple schedulers (which doesn't make much sense though),
// you can use each scheduler's spawn() method respectively.

extern void register_scheduler(WorkStealingScheduler&);
extern WorkStealingScheduler& get_scheduler();

namespace detail{ // internal use only 
// R is the value type of ForkJoinTask, i.e., the return type of F.
template< class R >
struct spawn_functor 
{
    // F is a callable object(function, functor, lambda, etc.) 
    // that can be invoked with (WorkStealingScheduler&, Args...)
    // todo: parse return type of F
    template<class F, class... Args, class=std::enable_if_t<(std::is_invocable<F, Args...>{})>>
    std::shared_ptr<WorkStealingScheduler::ForkJoinTask<R>> operator()(F&& f, Args&&... args) const
    {
        auto subtask=get_scheduler().create_forkjoin_task<R>();
        subtask->bind(std::forward<F>(f),std::forward<Args>(args)...);
        subtask->fork(); 
        return subtask;
    }
};

template< class R >
struct sched_aware_spawn_functor  // deprecated; reserved here only for compatible reason
{
    template<class F, class... Args, class=std::enable_if_t<(std::is_invocable<F, WorkStealingScheduler&, Args...>{})>>
    std::shared_ptr<WorkStealingScheduler::ForkJoinTask<R>> operator()(F&& f, WorkStealingScheduler&s, Args&&... args) const
    {
        auto subtask=s.create_forkjoin_task<R>();
        subtask->scheduler_aware_bind(std::forward<F>(f), std::forward<Args>(args)...);
        subtask->fork(); 
        return subtask;
    }
};

}

// fibonacci | async (12)  
template < class R >
static inline constexpr boost::hof::pipable_adaptor<detail::spawn_functor<R>> 
                        async{detail::spawn_functor<R>{}};

// Still too verbose, scheduler-aware async doesn't look better than spawn() method though.
// So it is deprecated, reserved here only for compatible reason.
// auto subtask=SumTree{}|sched_aware_async<int>(scheduler, x-1);
template < class R >
static inline constexpr boost::hof::pipable_adaptor<detail::sched_aware_spawn_functor<R>> 
                        sched_aware_async{detail::sched_aware_spawn_functor<R>{}};





}