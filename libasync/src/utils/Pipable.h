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

// Don't use this one!
// Use boost::hof, which actually supports multiple arguments

namespace wjp{

template<class F>
struct pipe_closure : F
{
    template<class... Xs>
    pipe_closure(Xs&&... xs) : F(std::forward<Xs>(xs)...)
    {}
};

template<class T, class F>
decltype(auto) operator|(T&& x, const pipe_closure<F>& p)
{
    return p(std::forward<T>(x));
}


// Helps pipable catch value perfectly
template<class T>
struct wrapper
{
    T value;
    template<class X, class=std::enable_if_t<(std::is_convertible<T, X>())>>
    wrapper(X&& x) : value(std::forward<X>(x))
    {}

    T get() const
    {
        return std::move(value);
    }
};

template<class T>
auto make_wrapper(T&& x)
{
    return wrapper<T>(std::forward<T>(x));
} 

template<class F>
auto make_pipe_closure(F f)
{
    return pipe_closure<F>(std::move(f));
}

// Supports more than one arguments
template<class F>
struct pipable
{
    template<class... Ts>
    auto operator()(Ts&&... xs) const
    {
        return make_pipe_closure(
            [](auto... ws){
                return [=](auto&& x) -> decltype(auto){
                    return F()(x, ws.get()...);
                };
            }(make_wrapper(std::forward<Ts>(xs)...)));
    }
};


template<class T, class F>
decltype(auto) operator|(T&& x, const pipable<F>& p)
{
    return F()(std::forward<T>(x));
}

struct add_one_f
{
    template<class T>
    auto operator()(T x) const
    {
        return x + 1;
    }
};

struct sum_f
{
    template<class T>
    auto operator()(T x, T y) const
    {
        return x + y;
    }
};

// C++17 inline variables
static inline const constexpr pipable<sum_f> sum = {};
static inline const constexpr pipable<add_one_f> add_one = {};



}