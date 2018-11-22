#include "WorkStealing.h"
#include <iostream>

namespace wjp::WorkStealing{

Worker::Worker(Scheduler&scheduler) : scheduler(scheduler){}

extern void thread_func(FixedThreadPool& pool, Worker& worker){
    std::cout<<"duh";

}




}