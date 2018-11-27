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
#include <memory>
#include <chrono>
#include <optional>

#include "ChaseLevDeque.h"
#include "SubmissionBuffer.h"
#include "Task.h"
#include "TimeUtilities.h"

namespace wjp{
class WorkStealingScheduler;
// Thread-local data handle of the worker thead
class WorkStealingWorker{
public:
	constexpr static auto idle_timeout=3s;  
    WorkStealingWorker(WorkStealingScheduler& scheduler);

    void routine(int index_of_me){
        auto task=scan_next_task(); //Take a task from local deque or steal one
        if(task==nullptr){ //Cannot find any task to run, check if it has been idle for too long. 
            if(!when_idle_begins.has_value()){ //Has always been busy
                when_idle_begins=now(); //Becomes idle
            }else{ //Has been idle before
                if(idle_for_too_long()){ //Idle timeout, seems unnecessary to busy spin any more, should block
                    block();
                }
            }
        }else{ //Task found
            #ifdef WJPTEST
            std::cout<<"worker"<<index_of_me<<" executes a task!"<<std::endl; 
            #endif
            when_idle_begins.reset(); //Becomes busy
            task->execute(); //Finish the task and wake up threads waiting for it.
        }
    }

    void push_to_deque(std::shared_ptr<Task> task){
        deque->push(task);
    }
    void push_to_buffer(std::shared_ptr<Task> task){
        buffer->submit(task);
    }

protected:
    void block(){
        //todo!!!! wait
        when_idle_begins.reset(); 
    }
    //Precondition: when_idle_begins has value
    bool idle_for_too_long(){
        return ms_elapsed(when_idle_begins.value())  >  idle_timeout ;
    }
    std::shared_ptr<Task> scan_next_task(){
        auto task=deque->take();
        if(task==nullptr){
            return steal_a_task();
        }else{
            return task;
        }
    }
    std::shared_ptr<Task> steal_a_task(){
        //@todo!!!!
        return nullptr;
    }
private:
    std::unique_ptr<ChaseLevDeque<Task>> deque;
    std::unique_ptr<SubmissionBuffer<Task>> buffer;
    std::reference_wrapper<WorkStealingScheduler> scheduler;
    std::optional<time_point> when_idle_begins; //Busy if empty, idle otherwise. 
};




}