#include "Internal.h"

using namespace wjp;

#define batch_size 16
#define workload_size 10000000

class A{
    void duh(){}
};

class B{
    void duh(){}
    int x;
};

void size_of_task(){
    int s0 = sizeof(int);
    int s1 = sizeof(std::mutex);
    int s2 = sizeof(std::condition_variable);
    int s3 = sizeof(std::function<void(void)>);
    int s4 = sizeof(Callable<int>);
    int s5 = sizeof(WorkStealingScheduler::FuturisticTask<int>);
    int s6 = sizeof(void*);

    std::cout<<"int="<<s0<<", mutex="<<s1<<", cv="<<s2<<", function="<<s3
    <<", Callable="<<s4<<", FuturisticTask="<<s5<<", ptr="<<s6<<", A="<< sizeof(A) 
    <<", B="<<sizeof(B)<<std::endl;

    // task must be more compact, therefore cv/mutex should be excluded from the kernel task
    // std::function should be avoided as well
}

int main(){
    size_of_task();
    return 0;
}