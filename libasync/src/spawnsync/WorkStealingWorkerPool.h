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
#include <vector>
#include <unordered_map>
#include <optional>
#include <iostream>
#include "WorkStealingWorker.h"
#include "RandomUtilities.h"

namespace wjp {

	class WorkStealingWorkerPool {
	public:
		WorkStealingWorkerPool(int nr_workers){
			for(int i=0; i<nr_workers; i++){
				workers.emplace_back(*this, i);
			}
			for(int i=0; i<nr_workers; i++){
				threads.emplace_back([this,i]{
					if(!started){}
					while(!terminating){
						workers[i].routine(); 
					}
					std::cout<<"worker"<<i<<" ended!\n";
				});
			}
		}

        ~WorkStealingWorkerPool() {
			std::cout<<"Start to tear down WorkerPool\n";
			terminating=true;  //Thread loop should always check if the pool has terminated
			for(auto& t : threads){
				t.join(); 
			}
			std::cout<<"WorkerPool Destroyed\n";
        }

		std::optional<int> current_thread_index()const noexcept{
			auto me=std::this_thread::get_id();
			for(int i=0;i<threads.size();i++){
				if(threads[i].get_id()==me)return i;
			}
			return {};
		}

		std::optional<std::reference_wrapper<WorkStealingWorker>> current_thread_handle()noexcept{
			auto index=current_thread_index();
			if(index){
				return std::ref(workers[index.value()]);
			}else{
				return {};
			}
		}

		std::reference_wrapper<WorkStealingWorker> randomly_pick_one()noexcept{
			int index=randinteger(0, threads.size()-1);
			return std::ref(workers[index]);
		}

		int nr_threads() const noexcept{
			return threads.size();
		}
		void start()noexcept{
			started=true;
		}
	 	bool terminating=false;  
		bool started=false;  
    private:
		std::vector<std::thread> threads {};
		std::vector<WorkStealingWorker> workers {};
	};

}

