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
#include "Task.h"

namespace wjp {
	class SpawnSyncTask;
	class SpawnSyncWorker;
	class SpawnSyncPool {
		friend class SpawnSyncTask;
	public:
		enum ThreadPoolState : int{
			RUNNING		= 0, // Accept new tasks and process queued tasks
			SHUTDOWN	= 1, // Stop accepting new tasks, but process queued tasks
			STOPPING	= 2, // Stop processing queued tasks, and tear down in-progress tasks
			STOPPED		= 3, // Already stopped, running on_stopped hook method
			TERMINATED	= 4, // on_stopped() has completed
		};
		SpawnSyncPool();
		~SpawnSyncPool() {}
		bool is_shutdown();
		bool is_terminiated();
	 	int get_state();
		void shutdown();
		std::list<std::shared_ptr<Runnable>> stop();
		void on_stopped();
		bool wait_till_terminated(std::chrono::milliseconds timeout);
		// If called from owning worker thread, push; otherwise, push_from_outsider. Convert Runnable to SpawnSyncTask. 
		std::shared_ptr<SpawnSyncTask> run(std::shared_ptr<Runnable>);  
		// to add template method: invoke Callable
	protected:
		void push_from_outsider(std::shared_ptr<SpawnSyncTask>);
		void wait_till_every_tasks_has_terminated();
		bool every_tasks_has_terminated();
		void activate_idle_worker_if_necessary(); 
	private:
		int state;
		std::vector<std::shared_ptr<SpawnSyncWorker>> workers;
	};

}

