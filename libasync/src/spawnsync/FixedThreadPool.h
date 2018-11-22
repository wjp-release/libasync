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

namespace wjp {

	// The supplied function must take FixedThreadPool* as its first argument.
	class FixedThreadPool {
	public:
		static int recommended_nr_thread(){
			return std::thread::hardware_concurrency()*2+1;
		}

		template <class Function, class... Args >
		FixedThreadPool(int nr_threads, Function&& f, Args&&... args){
			for(int i=0; i<nr_threads; i++){
				threads.emplace_back(std::forward<Function>(f), std::ref(*this), std::forward<Args>(args)...);
			}
		}

		template <class T, class Function, class... Args >
		FixedThreadPool(std::vector<T>& x, Function&& f, Args&&... args){
			for(int i=0; i<x.size(); i++){
				threads.emplace_back(std::forward<Function>(f), std::ref(*this), x[i], std::forward<Args>(args)...);
			}
		}

        ~FixedThreadPool() {
			for(auto& t : threads){
				t.join();
			}
        }

		bool contains_me(){
			auto me=std::this_thread::get_id();
			for(auto& t: threads){
				if(me==t.get_id()) return true;
			}
			return false;
		}

		int nr_threads(){
			return threads.size();
		}

    private:
		std::vector<std::thread> threads {};
	};

}

