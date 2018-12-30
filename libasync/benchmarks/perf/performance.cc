#include "Internal.h"

using namespace wjp;

#define batch_size 16
#define workload_size 10000000

void perf(){
    std::vector<int> v(workload_size);
    for(int i=0;i<workload_size;i++){
        v[i]=randint<0,10>();
    }
    // parallel sum
    WorkStealingScheduler scheduler; 
    register_scheduler(scheduler);
    int time_elapsed[batch_size];
    // warm up cpu (stop power saving) and cache
    libasync_sum<1000>(v.begin(), v.end());
    libasync_sum<1000>(v.begin(), v.end());
    // start perf sampling
    for(int i=0;i<batch_size;i++){
        time_point start=now();
        int sum=libasync_sum<1000>(v.begin(), v.end());
        time_elapsed[i]=ms_elapsed_count(start);
    }
    double avg=average(time_elapsed, batch_size);
    double stddev=standard_deviation(time_elapsed, batch_size);
    std::cout<<"avg time="<<avg<<"ms, stddev="<<stddev<<"ms\n";
}

int main(){
    perf();
    return 0;
}