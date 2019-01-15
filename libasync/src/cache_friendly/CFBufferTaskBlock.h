#pragma once

#include "CFConfig.h"
#include "CFTaskHeader.h"
#include <mutex>
#include <condition_variable>

namespace wjp::cf{
class Task;

// Root tasks(tasks in buffer) have true blocking sync method, i.e., they are actually getting blocked rather than busy spinning/helping during a sync().
// Note that we don't associate mtx & cv with vast vast majority of tasks that are generated in deque, since mtx & cv are too large(at least large enough to be cache-unfriendly) to fit in deque task blocks, while being useless in that context where blocking needs to be completely avoided anyway.

class BufferTaskBlock{
public:
    BufferTaskBlock() noexcept{
        header.isRoot=true;
    }
    TaskHeader&                 taskHeader() noexcept{
        return header;
    }
    void*                       taskAddress() noexcept{
        return address;
    }
    template < class T, class... Args >
    T*                          emplaceTask(Args&&... args){
        return new (address) T(std::forward<Args>(args)...);
    }
    template < class T = Task >
    T*                          taskPointer()noexcept{
        return reinterpret_cast<T*>(address);
    }
    template < class T = Task >
    T*                          taskReference()noexcept{
        return *reinterpret_cast<T*>(address);
    }
    template < class T >
    void                        destroyTask(){
        reinterpret_cast<T*>(address)->~T();
    }
    void                        setIndex(uint8_t index)noexcept{
        header.emplacerIndex=index;
    }
    std::mutex&                 getMutex() {return mtx;}
    std::condition_variable&    getConVar() {return cv;}
private:
    TaskHeader                  header; 
    char                        address[CacheLineSize-sizeof(TaskHeader)]; // padded to 128 bytes
    std::mutex                  mtx;
    std::condition_variable     cv;
};


}