#include "CFBuffer.h"
#include "CFTask.h"
#include "CFTaskHeader.h"
#include "CFConfig.h"

namespace wjp::cf{

// Return the least recently emplaced task
Task* TaskBuffer::steal()noexcept{
    std::lock_guard<BufferMutex> lk(mtx);
    if(endPosition>beginPosition){
        beginPosition++;
        Task* task = at(beginPosition-1).taskPointer();
        task->taskHeader().state=TaskHeader::StolenFromBuffer;
        return task;
    }else{
        return nullptr;
    }
}

void TaskBuffer::reclaim(Task* executed)noexcept{
    if constexpr(VeryVerboseDebug) println("TaskBuffer::reclaim!");
    executed->taskHeader().state=TaskHeader::Free;
}

int TaskBuffer::size()const noexcept{
    std::lock_guard<BufferMutex> lk(mtx);
    return endPosition-beginPosition;
}   


}