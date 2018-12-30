#include "CFBuffer.h"
#include "CFTask.h"

namespace wjp::cf{

// Return the least recently emplaced task
Task*                       TaskBuffer::steal(){
    std::lock_guard<BufferMutex> lk(mtx);
    if(endPosition>beginPosition){
        beginPosition++;
        return at(beginPosition-1).taskPointer();
    }else{
        return nullptr;
    }
}

int                         TaskBuffer::size(){
    std::lock_guard<BufferMutex> lk(mtx);
    return endPosition-beginPosition;
}   


}