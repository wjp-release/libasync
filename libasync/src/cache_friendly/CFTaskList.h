#pragma once

#include <cstdint>

namespace wjp::cf{

class Task;

class TaskList{
public:
    bool        empty() const noexcept;
    uint32_t    size()  const noexcept {return listSize;}
    void        pushBack(Task* target) noexcept;
    Task*       popBack() noexcept;
    void        pushFront(Task* target) noexcept;
    Task*       popFront() noexcept;
    void        remove(Task* target) noexcept;
    bool        contains(Task* target) const noexcept;
private:
    void        checkConsistency() noexcept;
    void        insertFirst(Task* target) noexcept;
    void        insertBetween(Task* prev, Task* next, Task* target) noexcept;
    Task*       first = nullptr;
    uint32_t    listSize = 0;
};


}