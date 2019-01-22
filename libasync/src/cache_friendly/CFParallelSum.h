#pragma once

#include "CFTask.h"
#include <numeric>
#include <cstdint>

namespace wjp::cf{

template <int GrainSize>
class ParallelSum: public Task {
public:
    const int* beg;
    const int* end;
    volatile int* sum;
    ParallelSum(const int* b, const int* e, volatile int* s):beg(b), end(e), sum(s){}
    std::string stats() override{
		return Task::stats()+"("+std::to_string((uint64_t)(end-beg))+")";
	}
	Task* compute() override{
        auto len = end - beg;
        if (len < GrainSize){
            *sum = std::accumulate(beg, end, 0);
            return nullptr;
        } 
        const int* mid = beg + len/2;
        volatile int x=-123;
        volatile int y=-123;
        spawn<ParallelSum>(mid,end,&x);
		spawnAsExec<ParallelSum>(beg,mid,&y);
        sync();
        *sum=x+y;
        if constexpr(VeryVerboseDebug) println(">> sum="+std::to_string(x)+"+"+std::to_string(y));
        return nullptr;
    }
};

template <int GrainSize>
int parallelSum(const int* arr, size_t n){
    volatile int sum;
	ParallelSum<GrainSize>* task=TaskPool::instance().emplaceRoot<ParallelSum<GrainSize>>(arr, arr+n, &sum);
    task->externalSync(); // external sync
    return sum;
}


}