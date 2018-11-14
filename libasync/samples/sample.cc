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

#include "sample.h"
#include <iostream>
#include <cassert>

#include "chaselev_deque.h"

using namespace wjp;
int main()
{
	// T must be
	struct A{
		A(const std::string& str):x(str){}
		std::string x;
	};

	chaselev_deque<A> q(3);  //cap=8
	auto t1=std::make_shared<A>("1");
	auto t2=std::make_shared<A>("2");
	auto t3=std::make_shared<A>("3");
	auto t4=std::make_shared<A>("4");
	auto t5=std::make_shared<A>("5");
	auto t6=std::make_shared<A>("6");
	auto t7=std::make_shared<A>("7");
	auto t8=std::make_shared<A>("8");
	auto t9=std::make_shared<A>("9");
	auto t10=std::make_shared<A>("10");
	auto t11=std::make_shared<A>("11");
	auto t12=std::make_shared<A>("12");
	q.push(t1);
	std::cout<<"push 1\n";
	q.print();
	q.push(t2);
	std::cout<<"push 2\n";
	q.print();
	q.push(t3);
	std::cout<<"push 3\n";
	q.print();
	q.push(t4);
	std::cout<<"push 4\n";
	q.print();
	q.push(t5);
	std::cout<<"push 5\n";
	q.print();
	q.push(t6);
	std::cout<<"push 6\n";
	q.print();
	q.push(t7);
	std::cout<<"push 7\n";
	q.print();
	q.push(t8);
	std::cout<<"push 8\n";
	q.print();
	q.push(t9);
	q.push(t10);
	std::cout<<"push 10\n";
	q.print();
	q.push(t11);
	q.push(t12);
	std::cout<<"push 12\n";
	q.print();

	std::shared_ptr<A> x=q.take();
	std::cout<<"take "<<x->x<<std::endl;
	x=q.take();
	std::cout<<"take "<<x->x<<std::endl;
	x=q.take();
	std::cout<<"take "<<x->x<<std::endl;

	q.print();

	x=q.steal();
	std::cout<<"steal "<<x<<std::endl;

	std::cout<<"steal "<<x->x<<std::endl;
	x=q.take();
	std::cout<<"take "<<x->x<<std::endl;

	q.print();

	x=q.take();
	std::cout<<"take "<<x->x<<std::endl;
	x=q.steal();
	std::cout<<"steal "<<x->x<<std::endl;
	x=q.steal();
	std::cout<<"steal "<<x->x<<std::endl;
	x=q.steal();
	std::cout<<"steal "<<x->x<<std::endl;
	x=q.steal();
	std::cout<<"steal "<<x->x<<std::endl;
	x=q.steal();
	q.print();
	std::cout<<"steal "<<x->x<<std::endl;
	q.print();
	x=q.take();
	std::cout<<"take "<<x->x<<std::endl;
	q.print();
	x=q.steal();
	std::cout<<"steal "<<x<<std::endl;
	q.print();

	std::cin.get();
	return 0;
}