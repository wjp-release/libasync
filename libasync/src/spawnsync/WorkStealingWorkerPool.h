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
#include <thread>
#include <unordered_map>
#include <optional>
#include <iostream>
#include "WorkStealingWorker.h"
#include "RandomUtilities.h"
#include "Array.h"

namespace wjp {

class WorkStealingWorkerPool {
public:
	// Creates <nr_workers> workers and same number of underlying threads. 
	// The newly created worker threads are initially blocking until start() is called. 
	WorkStealingWorkerPool(int nr_workers);
	// It's a blocking operation that waits until all threads complete an orderly-close.
	~WorkStealingWorkerPool();
	// Gets current thread's worker index.
	std::optional<int> current_thread_index()const noexcept;
	// Gets the requested worker.
	std::reference_wrapper<WorkStealingWorker> get_worker(int index);
	// Gets current thread's worker.
	std::optional<std::reference_wrapper<WorkStealingWorker>> current_thread_handle()noexcept;
	// Gets a randomly picked worker.
	std::reference_wrapper<WorkStealingWorker> randomly_pick_one()noexcept;
	// Gets the number of threads/workers.
	int nr_threads() const noexcept{return threads.capacity();}
	// Kicks it off.
	void start()noexcept{started=true;}
	// Shut it down.
	void terminate()noexcept{terminating=true;}
private:
	bool terminating=false;  
	bool started=false;  
	Array<std::thread> threads;   
	Array<WorkStealingWorker> workers;  
};

}

