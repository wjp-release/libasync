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

#include "CFConfig.h"
#include "CFWorker.h"
#include <optional>

namespace wjp::cf{

class TaskScheduler{
	TaskScheduler(){}
	~TaskScheduler(){}
	// std::optional<int> current_thread_index()const noexcept;
	// std::reference_wrapper<WorkStealingWorker> get_worker(int index);
	// std::optional<std::reference_wrapper<WorkStealingWorker>> current_thread_handle()noexcept;
	// std::reference_wrapper<WorkStealingWorker> randomly_pick_one()noexcept;
	// int nr_threads() const noexcept{return threads.capacity();}
	// void start()noexcept{started=true;}
	// void terminate()noexcept{terminating=true;}
	// void wake_all_sleeping_workers();
private:
	bool                            terminating=false;  
	bool                            started=false;  
    Worker                          workers[WorkerNumber];
};


}