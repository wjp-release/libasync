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

        // Note that nullptr_t assignment cannot be inherited, so we need to define it manually. Neither can nullptr_t construtor be inherited, but that makes perfect sense since SpawnSyncTask must have a scheduler reference ever since its construction. 
        SpawnSyncTask& operator=(std::nullptr_t)noexcept{
		    Callable<R>::operator=(nullptr);
		    return *this;
	    }
        
        void spawn(){
            auto worker=scheduler().pool->current_thread_handle();
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
        // Every copy/move constructor or assignment operator of Callable<R> is perfectly inherited and implicitly declared in SpawnSyncTask since this class meets all the requirements for the compiler to do so.
        // That's why scheduler has to be wrapped.
        std::reference_wrapper<WorkStealingScheduler> scheduler;
    };

    template < class R >
    std::shared_ptr<SpawnSyncTask<R>> create_task(){
        return std::make_shared<SpawnSyncTask<R>>(*this);
    }


private:
    std::unique_ptr<FixedThreadPool<WorkStealingWorker>> pool;
};



}