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
        return TaskPool::instance().emplaceLocally<T>(std::forward<Args>(args)...);
    }

    // Spawn a detached child task that's protected against stealing.
    // You can return this task in user-defined compute() function to promote it as a shortcut.
    template < class T, class... Args >  
    T*                      spawnDetachedAsExec(Args&&... args){  
        return TaskPool::instance().emplaceAsExec<T>(std::forward<Args>(args)...);
    }

    // Spawn a child task in deque in Ready state, 
    // which can be consumed by the owning worker,
    // or stolen by a hungry wild worker.
    template < class T, class... Args >  
    T*                      spawn(Args&&... args){

        T* child=TaskPool::instance().emplaceLocally<T>(std::forward<Args>(args)...);
        incRefCount();
        child->setParent(this);
        return child;
    }

    // Spawn a child task in deque in Exec state.
    // Exec tasks will always be executed by the local worker. 
    // They are scheduled with highest priority and safe from work-stealing.
    // Don't overuse spawnAsExec though, single worker cannot execute everything.
    template < class T, class... Args >  
    T*                      spawnAsExec(Args&&... args){  
        T* child=TaskPool::instance().emplaceAsExec<T>(std::forward<Args>(args)...);
        incRefCount();
        child->setParent(this);
        return child;
    }

    // Note that sync always waits for all child tasks. 
    // sync(Task*) could be easily misunderstood, so I decide not to expose it as public API.
    // Use spawnLastChildAndSync to benefit from shortcut.
    virtual void            sync(){  
        syncAndShortcut(nullptr);
    }

    // Execute the given task before fetching tasks from execList
    virtual void            syncAndShortcut(Task* shortcut); 

    void                    decRefCount() noexcept{
        taskHeader().refCount--; // atomic
    }
    void                    setRefCount(int r) noexcept
    {
        taskHeader().refCount=r;
    }
    void                    setParent(Task* parent){
        taskHeader().parent=parent;
    }
    bool                    notInList(){
        return taskHeader().next==nullptr && taskHeader().prev==nullptr;
    }
protected:
    virtual void            onComputeDone();
    // user-defined task routine
    virtual Task*           compute() = 0; 
    // shortcut must be emplaced as Exec; it will be executed immediately
private:
    void                    incRefCount() noexcept{
        taskHeader().refCount++;   
    }
};

class EmptyTask : public Task {
public:
    virtual Task*           compute() override {
        return nullptr;
    }
};



}