#include "CFTaskList.h"
#include "CFTaskHeader.h"
#include "CFTask.h"

namespace wjp::cf{

// private methods
void    TaskList::insertBetween(Task* prev, Task* next, Task* target)noexcept
{
    prev->taskHeader().next=target;
    next->taskHeader().prev=target;
    target->taskHeader().next=next;
    target->taskHeader().prev=prev;
    listSize++;
}
void    TaskList::insertFirst(Task* t)noexcept
{
    first=t;
    t->taskHeader().next=t;
    t->taskHeader().prev=t;
    listSize++;
}
// public methods
bool    TaskList::contains(Task* target) const noexcept
{
    if(empty()) return false;
    if(first==target) return true;
    for(Task*pos=first->taskHeader().next;pos!=first;pos=pos->taskHeader().next){
        if(pos==target) return true;
    }
    return false;
}
void    TaskList::remove(Task* t)noexcept
{
    if(t==nullptr) return;
    if(empty()) return;
    if(t->notInList()) return; // already removed
    if(listSize==1){
        if(t==first){
            first=nullptr;
            listSize=0;
            return;
        }
    }
    TaskHeader& header=t->taskHeader();
    if constexpr (EnableAssert){
        assert(header.prev!=nullptr&&header.next!=nullptr && "prev, next must be valid");
    }
    Task* prev=header.prev;
    Task* next=header.next;
    prev->taskHeader().next=next;
    next->taskHeader().prev=prev;
    header.prev=nullptr;
    header.next=nullptr;
    listSize--;
}
void    TaskList::pushBack(Task* t)noexcept
{
    if constexpr(EnableAssert) assert(t!=nullptr);
    if(empty()){
        insertFirst(t);
    }else{
        Task* last=first->taskHeader().prev;
        insertBetween(last,first,t);
    }
}
Task*   TaskList::popBack()noexcept
{
    if(empty()) return nullptr;
    Task* last=first->taskHeader().prev;
    remove(last);
    return last;
}
void    TaskList::pushFront(Task* t)noexcept
{
    pushBack(t);
    first=t;
}
Task*   TaskList::popFront()noexcept
{
    if(empty()) return nullptr;
    first=first->taskHeader().next;
    return popBack();
}


}