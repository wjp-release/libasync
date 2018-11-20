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

// Time, Random and other utilities

namespace wjp{

    using days = std::chrono::duration<int, std::ratio<3600*24,1>>;
    using time_point = std::chrono::high_resolution_clock::time_point;

    static inline time_point        now()
    {
        return std::chrono::high_resolution_clock::now();
    }
    
    static inline int64_t duration_ms_count(time_point start, time_point end)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    }

    static inline auto duration_ms(time_point start, time_point end)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    }

    static inline int64_t ms_elapsed_count(time_point start)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(now() - start).count();
    }

    static inline auto ms_elapsed(time_point start)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(now() - start);
    }

}