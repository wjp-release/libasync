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

#include "WorkStealingScheduler.h"
#include "WorkStealingWorker.h"
#include <iostream>

namespace wjp{

    WorkStealingWorker::WorkStealingWorker(WorkStealingScheduler&scheduler) : 
        scheduler(scheduler), 
        deque(std::make_unique<ChaseLevDeque<Task>>()), 
        buffer(std::make_unique<SubmissionBuffer<Task>>()){}
    
    void WorkStealingWorker::push_to_deque(std::shared_ptr<Task> task){
        std::cout<<"pushed to worker"<<index<<"'s deque.\n";
        deque->push(task);
    }

    void WorkStealingWorker::push_to_buffer(std::shared_ptr<Task> task){
        buffer->submit(task);
    }

    void WorkStealingWorker::routine(){
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
            std::cout<<"worker"<<index<<" executes a task!"<<std::endl; 
            when_idle_begins.reset(); //Becomes busy
            task->execute(); //Finish the task and wake up threads waiting for it.
        }
    }


}