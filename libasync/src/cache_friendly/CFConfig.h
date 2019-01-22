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

#include <stdint.h>
#include <iostream>
#include "TimeUtilities.h"

// Configurable constexpr values

namespace wjp::cf{

static constexpr uint8_t    WorkerNumber = 8;
static constexpr uint32_t   CacheLineSize = 128;  // 64B is a little bit too small for tasks
static constexpr uint32_t   TaskNumberPerWorker = 1024*1024;  
static constexpr uint32_t   BufferCapacity = 1024;
static constexpr uint32_t   DequeCapacity = TaskNumberPerWorker - BufferCapacity;
static constexpr int        InternalMonitorIntervalMS=1000;  // print rate

// worker yields after being idle for too long
static constexpr auto       WorkerIdleTimeout = 3s;  


// Turn it off if you can't even tolerate the performance loss of assertions
static constexpr bool       EnableAssert = true; 
static constexpr bool       MeasureInitTime = false;
static constexpr bool       SanityCheck = true;
static inline bool          SanityCheckSwitch=false;
static constexpr bool       VerboseDebug = true;
static constexpr bool       VeryVerboseDebug = false;
static constexpr bool       InformativeDebug = true;
static constexpr bool       SimplePause = false;

// If turned on, the user-defined returned task of Task::execute() will be executed immediately after the return. Note that this may delay the termniation of the pool until the AfterDeathShortcut-chain finishes.
static constexpr bool       EnableAfterDeathShortcut = false;

static constexpr bool       PrintTestTaskOutput = false;
static constexpr bool       EnableExternalSyncHelping = false; 

}

//#define EnableWorkerSleep

#define EnableInternalMonitor

//#define CFProfiling
