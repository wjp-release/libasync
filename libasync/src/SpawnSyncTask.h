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
#include "Future.h"
#include "ThreadPool.h"
#include <bitset>

namespace wjp {
	class SpawnSyncPool;
	class SpawnSyncWorker;
	class SpawnSyncTask : public Future {
	public:
		friend class SpawnSyncPool;
		friend class SpawnSyncWorker;
		
		// Default constructor should be called by ThreadPool users. The task is created as a specialization of Runnable. Calling submit() will bind this task with a worker and put it into the worker's submission task queue. 
		SpawnSyncTask() {}

		// The Constructor that constructs from worker should be called worker thread. This constructor guarantees that weak_ptr to worker is properly set.
		SpawnSyncTask(std::shared_ptr<SpawnSyncWorker> worker) : worker(worker){}
		
		// Task methods
		virtual void cancel() override;
		virtual void wait() override; // Should be called by non-worker thread. Seeks help_outsider_wait's help before actual condition wait. 
		virtual void wait(std::chrono::milliseconds timeout) override;
		virtual bool is_canceled() override {return state[0];}
		virtual bool is_finished() override {return state[1];}
		virtual void finish() override {state[1]=true;}

		// SpawnSyncTask methods
		// Tries to run(), catch exception, finish with proper status.
		virtual void exec()=0; 
		
		// Should be called by worker thread only. It tries take back the task and execute it directly before calling worker's wait_to_sync() to actually wait.
		void sync();

		// Should be called by worker thread only.
		void spawn();

	private:
		// Tries to exploit idle non-worker thread that waits for this task to directly execute it.
		void help_outsider_wait();
		std::weak_ptr<SpawnSyncWorker> worker;
		std::bitset<2> state; // is_canceled, is_finished
		std::condition_variable condition_finished;
		std::mutex mtx;
	};
}

