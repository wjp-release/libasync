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

#include <functional>

namespace wjp {

// Universal Callable
template< class R>
class Callable{
public:
	// Constructors
	Callable(){}
	template< class F, class... Args >
	Callable(F&& f, Args&&... args ){
		callable_function = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
	}
	// Operators
	R operator()(){
		return callable_function();
	}
	Callable& operator=( std::nullptr_t ){
		callable_function=nullptr;
		return *this;
	}
	Callable<R>& operator=( Callable<R>&& other ){
		callable_function=std::move(other.callable_function);
		return *this;
	}
	Callable& operator=(const Callable& other ){
		callable_function=other.callable_function;
		return *this;
	}	
	explicit operator bool() const noexcept{
		return (bool)callable_function;
	}
private:
	std::function<R()> callable_function;
};

}

