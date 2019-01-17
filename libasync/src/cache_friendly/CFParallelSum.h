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
    int* sum;
    ParallelSum(const int* b, const int* e, int* s):beg(b), end(e), sum(s){}
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
        int x,y;
        spawn<ParallelSum>(mid,end,&x);
		spawnAsExec<ParallelSum>(beg,mid,&y);
        sync();
        *sum=x+y;
        if constexpr(VerboseDebug) println(">> sum="+std::to_string(x)+"+"+std::to_string(y));
        return nullptr;
    }
};

template <int GrainSize>
int parallelSum(const int* arr, size_t n){
    int sum;
	ParallelSum<GrainSize>* task=TaskPool::instance().emplaceRoot<ParallelSum<GrainSize>>(arr, arr+n, &sum);
    task->sync();
    return sum;
}


}