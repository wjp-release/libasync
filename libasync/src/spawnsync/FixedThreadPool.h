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

#include "RandomUtilities.h"

namespace wjp {

	template < class U >   // U is the type of thread local handle.
	class FixedThreadPool {
	public:

		// Constructs from existing thread-local handles of worker threads.
		// The given function must take FixedThreadPool* as its first argument, and the thread local handle U& as its second argument. The given vector's ownership will be transferred to this object. 
		template <class Function, class... Args >
		FixedThreadPool(std::vector<U>& x, Function&& f, Args&&... args){
			for(int i=0; i<x.size(); i++){
				threads.emplace_back(std::forward<Function>(f), std::ref(*this), std::ref(x.at(i)), std::forward<Args>(args)...);
				tid_to_index[threads[i].get_id()]=i;
			}
			thread_local_handles.swap(x);
		}

        ~FixedThreadPool() {
			for(auto& t : threads){
				t.join();
			}
        }

		std::optional<int> current_thread_index(){
			auto me=std::this_thread::get_id();
			if(tid_to_index.count(me)) return tid_to_index[me];
			return {};
		}


		std::optional<std::reference_wrapper<U>> current_thread_handle(){
			auto index=current_thread_index();
			if(index){
				return std::ref(thread_local_handles[index.value()]);
			}else{
				return {};
			}
		}

		std::reference_wrapper<U> randomly_pick_one(){
			int index=randinteger(0, threads.size()-1);
			return std::ref(thread_local_handles[index]);
		}

		int nr_threads(){
			return threads.size();
		}

    private:
		std::vector<std::thread> threads {};
		std::vector<U> thread_local_handles {};
		std::unordered_map<std::thread::id, int> tid_to_index{};
	};

}

