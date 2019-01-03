#pragma once

#include <cstdint>

namespace wjp::cf{

class Task;

class TaskList{
public:
    bool        empty() const noexcept {return first==nullptr;}
    uint32_t    size()  const noexcept {return listSize;}
    void        pushBack(Task* target);
    Task*       popBack();
    void        pushFront(Task* target);
    Task*       popFront();
private:
    void        remove(Task* target);
    void        insertFirst(Task* target);
    void        insertBetween(Task* prev, Task* next, Task* target);
    Task*       first = nullptr;
    uint32_t    listSize = 0;
};


}