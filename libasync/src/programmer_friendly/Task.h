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
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include "Config.h"

namespace wjp {

	class Task : public std::enable_shared_from_this<Task>  
	{
    public:
		virtual void execute()=0; // sched-->done
		virtual void compute()=0; // does not alter the task's state; used by freelance tasks
		virtual void wait() = 0;
		virtual void wait(std::chrono::milliseconds timeout) = 0;
		virtual bool is_finished() const noexcept = 0;  
		virtual bool cancel() = 0; // Returns true on success
		virtual bool is_canceled() const noexcept = 0;  
		virtual void submit() = 0;
		virtual void to_sched() = 0;
		virtual ~Task() {}
        struct Sync{ 
            Sync(){} 
            Sync(const Sync&){} 
            Sync(Sync&&){} 
            Sync& operator=(Sync&&){return *this;} 
            Sync& operator=(const Sync&s){return *this;}
            mutable std::mutex mtx; // Sync wait 
			#ifdef WITH_CANCEL
			mutable std::shared_mutex cancel_mtx; // Sync cancel
			#endif
            mutable std::condition_variable cv;  // Notified on finished
        };
        mutable Sync sync;  // copy/move constructible & copy/move assignable wrapper of mtx, cv
	};
}
