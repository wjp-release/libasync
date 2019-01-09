#include "CFTaskList.h"
#include "CFTaskHeader.h"
#include "CFTask.h"

namespace wjp::cf{

// private methods
void    TaskList::insertBetween(Task* prev, Task* next, Task* target)noexcept
{
    assert(prev!=nullptr&&next!=nullptr&&target!=nullptr);
    prev->taskHeader().next=target;
    next->taskHeader().prev=target;
    target->taskHeader().next=next;
    target->taskHeader().prev=prev;
    listSize++;
}
void    TaskList::insertFirst(Task* t)noexcept
{
    assert(t!=nullptr&&first==nullptr&&listSize==0);
    first=t;
    t->taskHeader().next=t;
    t->taskHeader().prev=t;
    listSize=1;
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
    if(t->notInList()) return; 
    if(listSize==1){
        if(t==first){
            first=nullptr;
            listSize=0;
            return;
        }else{
            if constexpr (InformativeDebug) println("removing non-existing task");
            return;
        }
    }
    if constexpr (SanityCheck){
        assert(contains(t));
    }
    if(t==first && listSize>1){ 
        first=first->taskHeader().next; // update first if old first get removed
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
    if constexpr (SanityCheck){
        checkConsistency();
    }
}

bool        TaskList::empty() const noexcept {
    if constexpr(EnableAssert){
        if(listSize==0) assert(first==nullptr);
        else assert(first!=nullptr);
    } 
    return listSize==0;
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
    if constexpr (SanityCheck){
        if(SanityCheckSwitch) checkConsistency();
    }
}
Task*   TaskList::popBack()noexcept
{
    if(empty()) return nullptr;
    if(listSize==1){
        if constexpr(EnableAssert) assert(first!=nullptr);
        Task* tmp=first;
        first=nullptr;
        listSize=0;
        return tmp;
    }
    Task* last=first->taskHeader().prev;
    remove(last);
    if constexpr (SanityCheck){
        checkConsistency();
    }
    return last;
}

void    TaskList::checkConsistency() noexcept
{
    if(listSize<0) assert(false);
    if(listSize==0){
        assert(first==nullptr);
        return;
    }
    if(listSize==1){
        assert(first!=nullptr && first->taskHeader().next==first 
        && first->taskHeader().prev==first );
        return;
    }
    assert(first!=nullptr && first->taskHeader().next!=nullptr && first->taskHeader().prev!=nullptr);
    uint64_t num=1;
    for(Task*pos=first->taskHeader().next;pos!=first;pos=pos->taskHeader().next){
        num++;
    }
    assert(num==listSize);
}

void    TaskList::pushFront(Task* t)noexcept
{
    pushBack(t);
    first=t;
    if constexpr (SanityCheck){
        checkConsistency();
    }
}

Task*   TaskList::popFront()noexcept
{
    if(empty()) return nullptr;
    if(listSize==1){
        if constexpr(EnableAssert) assert(first!=nullptr);
        Task* tmp=first;
        first=nullptr;
        listSize=0;
        return tmp;
    }
    first=first->taskHeader().next;
    if constexpr (SanityCheck){
        checkConsistency();
    }
    return popBack();
}


}