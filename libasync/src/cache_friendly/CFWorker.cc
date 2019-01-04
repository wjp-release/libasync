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

#include "CFWorker.h"
#include "ConcurrentPrint.h"
#include "CFTaskHeader.h"
#include "CFTask.h"

#include <sstream>

namespace wjp::cf{

void Worker::routine()
{

}

void Worker::reclaim(Task* executed) noexcept
{
    if(executed->taskHeader().state==TaskHeader::StolenFromBuffer){
        buffer.reclaim(executed);
    }else{
        deque.reclaim(executed);
    }
}


std::string Worker::stat()
{
    std::stringstream ss;
    ss<<"worker "<<index;
    #ifdef EnableWorkerSleep
    bool is_idle_=is_idle();
    ss<<": is_blocked="<< blocked <<", is_idle="<<is_idle_;
    if(is_idle_){
        ss<<"("<< ms_elapsed_count(when_idle_begins.value()) <<"ms)";
    }
    #endif
    ss<<", deque freeListSize="<<deque.freeListSize()<<", buffer size="<<buffer.size();
    return ss.str();
}

}