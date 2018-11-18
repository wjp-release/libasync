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
		// Tells pool to cancel itself, though the task has no control when it will be actually released
		virtual void cancel() override;
		// Wait until it 
		virtual void wait() override;

		virtual void wait(std::chrono::milliseconds timeout) override;

		virtual bool is_canceled() override
		{
			return state[0];
		}

		virtual bool is_finished() override
		{
			return state[1]; 
		}
	private:
		std::weak_ptr<SpawnSyncPool> pool;
		std::weak_ptr<SpawnSyncWorker> worker;
		std::bitset<2> state; // is_canceled, is_finished
	};
}

