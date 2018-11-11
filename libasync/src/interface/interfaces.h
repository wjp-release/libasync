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
#include <chrono>

namespace wjp {

	class runnable {
	public:
		virtual void run() = 0;
		virtual ~runnable() {}
	};

	class task : public runnable {
	public:
		virtual void cancel(bool allow_interrupt) = 0;
		virtual bool is_canceled() = 0;
		virtual bool is_finished() = 0;
		virtual void wait() = 0;
		virtual void wait(std::chrono::milliseconds timeout) = 0;
		virtual ~task() {}
	};

	class provider {
	public:
		virtual ~provider() {}
		// stop accepting new tasks, but process queued tasks
		virtual void shutdown() = 0;
		// shutdown, interrupt executing tasks, clear queued tasks, return them 
		virtual std::shared_ptr<runnable> stop() = 0;
		virtual bool is_shutdown() = 0;
		virtual bool is_terminiated() = 0;
		virtual bool wait_till_terminated(std::chrono::milliseconds timeout) = 0;
		virtual std::shared_ptr<task> run(std::shared_ptr<runnable>) = 0;  
		virtual std::list<std::shared_ptr<task>> all(std::list<std::shared_ptr<runnable>>) = 0;
		// the returned task will wait for the first runnable to finish successfully
		virtual std::shared_ptr<task> any(std::list<std::shared_ptr<runnable>>) = 0;

	};



}
