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

#include <cstdint>
#include "CFConfig.h"
#include "CFTaskHeader.h"
#include "CFPool.h"

namespace wjp::cf{

class Task{
public:
    TaskHeader&             taskHeader() const{
        return reinterpret_cast<TaskHeader*>(const_cast<Task*>(this))[-1];
    }
    virtual Task*           execute() = 0; // return the latest generated child task
    
    // Spawn a child task in deque in Ready state, 
    // which can be consumed by the owning worker,
    // or stolen by a hungry wild worker.
    template < class T, class... Args >  
    T*                      spawn(Args&&... args){
        return TaskPool::instance().emplaceLocally(std::forward<Args>(args)...);
    }

    // Spawn a child task in deque in Exec state.
    // Exec tasks will always be executed by the local worker. 
    // They are scheduled with highest priority and safe from work-stealing.
    // Don't overuse spawnAsExec though, single worker cannot execute everything.
    template < class T, class... Args >  
    T*                      spawnAsExec(Args&&... args){  
        return TaskPool::instance().emplaceAsExec(std::forward<Args>(args)...);
    }

    // Why not return the task? 
    // sync(task) will always finish this task. 
    // So when you are able to return it, it's already destroyed.
    template < class T, class... Args >  
    void                    spawnLastChildAndSync(Args&&... args){
        T* task=TaskPool::instance().emplaceAsExec(std::forward<Args>(args)...);
        sync(task);
    }
    // Note that sync always waits for all child tasks. 
    // sync(Task*) could be easily misunderstood, so I decide not to expose it as public API.
    // Use spawnLastChildAndSync to benefit from shortcut.
    virtual void            sync(){  
        sync(nullptr);
    }
    void                    setRefCount(int r)
    {
        taskHeader().refCount=r;
    }
protected:
    // shortcut must be emplaced as Exec; it will be executed immediately
    virtual void            sync(Task* shortcut); 
};

class EmptyTask : public Task {
public:
    Task*                   execute() override {
        return nullptr;
    }
};


}