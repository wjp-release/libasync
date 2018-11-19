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

#include "Common.h"
#include "ThreadPool.h"

namespace wjp {
	class SpawnSyncTask;
	class SpawnSyncWorker;
	class SpawnSyncPool : public ThreadPool {
	public:
		friend class SpawnSyncTask;
		SpawnSyncPool();
		// Overrided functions from ThreadPool
		virtual void shutdown()override;
		virtual std::list<std::shared_ptr<Runnable>> stop()override;
		virtual void on_stopped()override;
		virtual bool wait_till_terminated(std::chrono::milliseconds timeout)override;
		// If called from owning worker thread, push; otherwise, push_from_outsider. Convert Runnable to SpawnSyncTask. 
		virtual std::shared_ptr<Task> run(std::shared_ptr<Runnable>)override;  
	protected:
		void push_from_outsider(std::shared_ptr<SpawnSyncTask>);
		void wait_till_every_tasks_has_terminated();
		bool every_tasks_has_terminated();
		void activate_idle_worker_if_necessary(); 
	private:
		std::vector<std::shared_ptr<SpawnSyncWorker>> workers;
	};

}

