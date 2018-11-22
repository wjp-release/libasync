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
#include "ChaseLevDeque.h"
#include "SubmissionBuffer.h"
#include "TimeUtilities.h"

/*
	Each worker maintains:
	1) a worker thread
	2) a submission task queue
	3) a local task queue

*/
namespace wjp {
	class SpawnSyncTask;
	class SpawnSyncPool;
	class SpawnSyncWorker {
	public:
		friend class SpawnSyncTask;
		SpawnSyncWorker(SpawnSyncPool&);

		virtual ~SpawnSyncWorker();
		
		// Pushes a task into local task queue.
		void push(std::shared_ptr<SpawnSyncTask>);
		
		// Takes the latest pushed task from local task queue.
		std::shared_ptr<SpawnSyncTask> take(); 
		
		// Offers a least recently pushed task to stealers.
		std::shared_ptr<SpawnSyncTask> steal(); 
		
		// Pushes a task into submission task queue. Should be called by a non-worker thread.
		void submit(std::shared_ptr<SpawnSyncTask>);
		
		// Takes the latest pushed task from submission task queue. Should be called by a non-worker thread.
		std::shared_ptr<SpawnSyncTask> undo_submit();

		// Creates the worker thread that runs worker_routine, set thread id.
		void start();

		// Blocks the worker thread untill it is waked by SpawnSyncPool
		void stall();

		// 
		void shutdown();
		

	protected:
		// Helps a task to cancel.
		virtual void cancel(std::shared_ptr<Task>);  

		// Helps a task to sync. 
		// Optimization 1: Try to steal back child tasks of the target task from other workers to avoid compromising parallelism
		// Optimization 2: Try to help CountedDown tasks 
		// Optimization 3: Try to find the specified task, remove it from queue, and execute it 
		virtual void sync(std::shared_ptr<SpawnSyncTask> target_task, std::chrono::milliseconds timeout);

	private:
		// Try to steal from a random victim. If nothing could be stolen, enter the idle queue. If the worker has been idle, try to enter the sleeping queue.
		void worker_routine(); 
		// The worker will check its own local & submitted task queue, if nothing found, then scan other workers.
		bool find_next_task(); 
		// Try to steal from the same worker multiple times to exploit temporal locality.
		void execute_stolen_tasks();
		SpawnSyncPool& pool; 
		ChaseLevDeque<SpawnSyncTask> local_task_queue;
		SubmissionBuffer<SpawnSyncTask> submission_task_queue;
		bool is_shutdown = false;
		bool is_stalling = false;
		std::optional<wjp::time_point> became_idle = std::nullopt;
		std::thread worker_thread;
		std::condition_variable condition_stall;
		std::mutex mtx;
		static constexpr uint64_t idle_timeout = 10000; // A task can stay idle for at most 10 seconds, then it will stall.

	};

}


