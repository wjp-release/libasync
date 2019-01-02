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

#include "CFDeque.h"
#include "CFTask.h"
#include "CFTaskHeader.h"

namespace wjp::cf{

// Return the least recently emplaced task
Task*                       TaskDeque::steal(){
    std::lock_guard<DequeMutex> lk(mtx);
    if(endPosition>beginPosition){
        beginPosition++;
        return at(beginPosition-1).taskPointer();
    }else{
        return nullptr;
    }
}

// Return the most recently emplaced task
Task*                       TaskDeque::take(){
    std::lock_guard<DequeMutex> lk(mtx);
    if(endPosition>beginPosition){
        int pos=endPosition-1;
        // todo!
        return at(pos).taskPointer();
    }else{
        return nullptr;
    }
}

int                         TaskDeque::size(){
    std::lock_guard<DequeMutex> lk(mtx);
    return endPosition-beginPosition;
}   


void                        TaskDeque::freeListPush(Task* t){
    if(freeListEmpty()){
        firstFreeTask=t;
        t->taskHeader().next=t;
        t->taskHeader().prev=t;
    }else{
        Task* last=firstFreeTask->taskHeader().prev;
        last->taskHeader().next=t;
        t->taskHeader().prev=last;
        t->taskHeader().next=firstFreeTask;
    }
}
Task*                       TaskDeque::freeListPop(){
    if(freeListEmpty()) return nullptr;
    Task* last=firstFreeTask->taskHeader().prev;
    Task* prev_last=last->taskHeader().prev;
    prev_last->taskHeader().next=firstFreeTask;
    firstFreeTask->taskHeader().prev=prev_last;
    return last;
}

}