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

#include <any>
#include "Internal.h"

namespace wjp{

    struct A{
        A(const std::string& str):x(str){}
        std::string x;
    };
    static inline void deque_sample(){
        ChaseLevDeque<A>* q=new ChaseLevDeque<A>(4);  
        for(int i=1;i<=100;i++){
            q->push(std::make_shared<A>(std::to_string(i)));
            std::cout<<">>> ";
            q->print();
        }	
        delete q;
    }

	


	static inline void 	cpplatest() {
		std::any a = 1;
		std::cout << std::any_cast<int>(a) << std::endl;
	}

	

    struct Bobo{
        Bobo(int n) : threads(n), workers(n)
        {
            for(int i=0;i<n;i++){
                workers.emplace_back("worker"+std::to_string(i));
            }
            for(int i=0;i<n;i++){
                threads.emplace_back([i]{
                    // std::cout<<"bobo"<<this->workers[i].x<<"\n";
                    std::cout<<"bobo"<<i<<"\n";
                });
            }
        }
        ~Bobo(){
            for(auto& t:threads){
                t.join();
            }
        }
        Array<std::thread> threads;
        Array<A> workers;
    };

	static inline void arraytest() {
        {
            WorkStealingWorkerPool pool(9);
            pool.start();
        }
	}


    static inline void worksteal_test(){
        WorkStealingScheduler scheduler;
        struct bee{
            int operator()(int x, int y, int z){
                return x*100+y*10+z;
            }
        };
        auto w = scheduler.create_futuristic_task<int>();
        w->bind(bee{}, 1,2,3);
        scheduler.submit(w);
        try{
            auto res=w->get();
            int x=res.value_or(-100);
            std::cout<<"now we get x="<<x<<std::endl;
            assert(x==123);
        }catch(std::exception& e){
            std::cerr<<"exception: "<<e.what()<<std::endl;
        }catch(...){
            std::cerr<<"unknown exception!"<<std::endl;
        }
    }


}