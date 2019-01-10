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
#include <string>

namespace wjp::cf{
class Worker;
class Task{
public:
    // Return human-readable unique task identity info
    virtual std::string     stats(){
        return "Task<"+std::to_string(reinterpret_cast<uint64_t>(this))+">";
    }
    TaskHeader&             taskHeader() const noexcept{
        return reinterpret_cast<TaskHeader*>(const_cast<Task*>(this))[-1];
    }
    // Run user-defined routine and release the task.
    // Return a child task that is (1) detached, (2) in execList as the shortcut after parent task is executed.
    // If you don't have a detached child task that must be run immediately after the parent task, return nullptr.
    Task*                   execute();
    
    // Parent tasks' sync()/syncWithShortcut() doesn't wait for detached tasks.
    // A detached child task does not point back to its parent. 
    // It does not have any effect on parent task's refcount as well.
    template < class T, class... Args >  
    T*                      spawnDetached(Args&&... args){
        return TaskPool::instance().emplaceInto<T>(TaskPool::instance().currentThreadIndex().value_or(taskHeader().emplacerIndex),
        std::forward<Args>(args)...);
    }

    // Spawn a detached child task that's protected against stealing.
    // You can return this task in user-defined compute() function to promote it as a shortcut.
    template < class T, class... Args >  
    T*                      spawnDetachedAsExec(Args&&... args){ 
        return TaskPool::instance().emplaceAsExecInto<T>(TaskPool::instance().currentThreadIndex().value_or(taskHeader().emplacerIndex),
        std::forward<Args>(args)...);
    }

    // Spawn a child task in deque in Ready state, 
    // which can be consumed by the owning worker,
    // or stolen by a hungry wild worker.
    template < class T, class... Args >  
    T*                      spawn(Args&&... args){
        return TaskPool::instance().emplaceIntoAndInit<T>(TaskPool::instance().currentThreadIndex().value_or(taskHeader().emplacerIndex), 
        this, 
        std::forward<Args>(args)...);
    }

    // Spawn a child task in deque in Exec state.
    // Exec tasks will always be executed by the local worker. 
    // They are scheduled with highest priority and safe from work-stealing.
    // Don't overuse spawnAsExec though, single worker cannot execute everything.
    template < class T, class... Args >  
    T*                      spawnAsExec(Args&&... args){  
        return TaskPool::instance().emplaceAsExecIntoAndInit<T>(TaskPool::instance().currentThreadIndex().value_or(taskHeader().emplacerIndex), 
        this, 
        std::forward<Args>(args)...);
    }

    // Note that sync always waits for all child tasks. 
    void                    sync();

    void                    decRefCount() noexcept{
        taskHeader().refCount--; // atomic
    }
    void                    setRefCount(int r) noexcept
    {
        taskHeader().refCount=r;
    }
    void                    setParentAndIncRefCount(Task* parent){
        taskHeader().refCount++;
        taskHeader().parent=parent;
    }
    bool                    notInList(){
        return taskHeader().next==nullptr && taskHeader().prev==nullptr;
    }
protected:
    void                    localSync(Worker&worker);
    void                    externalSync();
    virtual void            onComputeDone();
    // user-defined task routine
    virtual Task*           compute() = 0; 
    // shortcut must be emplaced as Exec; it will be executed immediately
};

class EmptyTask : public Task {
public:
    virtual Task*           compute() override {
        return nullptr;
    }
};



}