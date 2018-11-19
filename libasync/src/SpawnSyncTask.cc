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

#include "SpawnSyncTask.h"
#include "SpawnSyncPool.h"

namespace wjp {

	void SpawnSyncTask::cancel()
	{
		state[0] = true;
		if(auto worker_shared=worker.lock())
			worker_shared->cancel(shared_from_this());
	}
	
	void SpawnSyncTask::wait() 
	{
		// todo permission check
		help_outsider_wait(); 
		std::unique_lock<std::mutex> lk(mtx);
		condition_finished.wait(lk, [this]{return is_finished();});
	}

	void SpawnSyncTask::wait(std::chrono::milliseconds timeout) 
	{
		// todo permission check
		help_outsider_wait(); 
		std::unique_lock<std::mutex> lk(mtx);
		condition_finished.wait_for(lk, timeout, [this]{return is_finished();});
	}

	void SpawnSyncTask::sync()
	{

	}

	void SpawnSyncTask::spawn()
	{

	}

	// Why don't you run the task by yourself if you have time waiting here?
	void help_outsider_wait()
	{
		// try to undo submit
	}

}