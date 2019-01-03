#include "CFTaskList.h"
#include "CFTaskHeader.h"
#include "CFTask.h"

namespace wjp::cf{

// private methods
void    TaskList::insertBetween(Task* prev, Task* next, Task* target)
{
    prev->taskHeader().next=target;
    next->taskHeader().prev=target;
    target->taskHeader().next=next;
    target->taskHeader().prev=prev;
    listSize++;
}
void    TaskList::insertFirst(Task* t){
    first=t;
    t->taskHeader().next=t;
    t->taskHeader().prev=t;
    listSize++;
}
void    TaskList::remove(Task* t){
    Task* prev=t->taskHeader().prev;
    Task* next=t->taskHeader().next;
    prev->taskHeader().next=next;
    next->taskHeader().prev=prev;
    listSize--;
}
// public methods
void    TaskList::pushBack(Task* t){
    if constexpr(EnableAssert) assert(t!=nullptr);
    if(empty()){
        insertFirst(t);
    }else{
        Task* last=first->taskHeader().prev;
        insertBetween(last,first,t);
    }
}
Task*   TaskList::popBack(){
    if(empty()) return nullptr;
    Task* last=first->taskHeader().prev;
    remove(last);
    return last;
}
void    TaskList::pushFront(Task* t){
    pushBack(t);
    first=t;
}
Task*   TaskList::popFront(){
    if(empty()) return nullptr;
    first=first->taskHeader().next;
    return popBack();
}


}