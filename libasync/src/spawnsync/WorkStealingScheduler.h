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
#include <optional>
#include <vector>

#include "FixedThreadPool.h"
#include "ChaseLevDeque.h"
#include "Callable.h"
#include "ThreadUtilities.h"
#include "WorkStealingWorker.h"
#include "WorkStealingRoutine.h"

namespace wjp{

class WorkStealingScheduler{
public:
    // Starts running on creation
    WorkStealingScheduler();

    template< class R >
    class SpawnSyncTask : public Callable<R>{
    public:

        SpawnSyncTask(WorkStealingScheduler& sched):scheduler(sched)
        {}
        
        void spawn(){
            auto worker=scheduler.pool->current_thread_handle();
            if(worker){ //Called from a worker thread

            }else{ //Called from an external thread

            }
        }
        void sync(){
        
        }

        std::optional<R> get(){
            sync();
            return value;
        }

    private:
        std::optional<R> value;
        WorkStealingScheduler& scheduler;
    };

    template < class R >
    std::shared_ptr<SpawnSyncTask<R>> create_task(){
        return std::make_shared<SpawnSyncTask<R>>(*this);
    }


private:
    std::unique_ptr<FixedThreadPool<WorkStealingWorker>> pool;
};



}