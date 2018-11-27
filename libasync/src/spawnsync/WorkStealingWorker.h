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
class WorkStealingWorkerPool;
// Thread-local data handle of the worker thead
class WorkStealingWorker{
public:
	constexpr static auto idle_timeout=3s;  
    WorkStealingWorker(WorkStealingWorkerPool& pool, int index);
    void routine();
    void push_to_deque(std::shared_ptr<Task> task);
    void push_to_buffer(std::shared_ptr<Task> task);
protected:
    void block(){
        //todo!!!! wait
        std::cout<<"worker"<<index<<" idle!"<<std::endl;
        when_idle_begins.reset(); 
    }
    //Precondition: when_idle_begins has value
    bool idle_for_too_long(){
        return ms_elapsed(when_idle_begins.value())  >  idle_timeout ;
    }
    std::shared_ptr<Task> scan_next_task(){
        //auto task=deque->take();  //test!
        auto task=buffer->steal();
        if(task==nullptr){
            return steal_a_task();
        }else{
            std::cout<<"\nworker"<<index<<" found a task!!\n";
            return task;
        }
    }
    std::shared_ptr<Task> steal_a_task(){
        //@todo!!!!
        return nullptr;
    }
private:    
    int index=-2;
    std::unique_ptr<ChaseLevDeque<Task>> deque;
    std::unique_ptr<SubmissionBuffer<Task>> buffer;
    std::reference_wrapper<WorkStealingWorkerPool> pool;
    std::optional<time_point> when_idle_begins; //Busy if empty, idle otherwise. 
};




}