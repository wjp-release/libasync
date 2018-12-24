#include "parallel_sum_benchmark.h"
#include "Internal.h"
#include "parallel_sum_tbb.h"
#include "parallel_sum_stdcpp.h"
#include "test_tbb.h"
using namespace wjp;

#define workload_size 10000000

void parallel_sum_benchmark_grainsize_1000()
{
    // init workload 100k 
    std::vector<int> v(workload_size);
    for(int i=0;i<workload_size;i++){
        v[i]=randint<0,10>();
    }
    // serial sum
    time_point time_start=now();
    int sum_of_v=0;
    for(int i=0;i<workload_size;i++){
        sum_of_v+=v[i];
    }
    int time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"forloop sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";
    
    // std::accumulate sum
    time_start=now();
    sum_of_v=std::accumulate(v.begin(), v.end(), 0);
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"std::accumulate sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";

    // tbb::parallel_reduce sum
    time_start=now();
    sum_of_v=tbb_parallel_sum<1000>(v.data(), v.size());
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"tbb sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";

    // std::async(fork/join) sum 
    time_start=now();
    sum_of_v=std_parallel_sum<1000>(v.begin(), v.end());
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"std::async sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";

    // tbb(fork/join) sum
    time_start=now();
    sum_of_v=tbb_fjtask_sum<1000>(v.data(), v.size());
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"tbb fork/join sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";

    // libasync init scheduler
    WorkStealingScheduler scheduler;
    register_scheduler(scheduler);

    // libasync(fork/join) sum
    time_start=now();
    sum_of_v=libasync_sum<1000>(v.begin(), v.end());
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"libasync sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";
}


void parallel_sum_benchmark_grainsize_5000()
{
    // init workload 100k 
    std::vector<int> v(workload_size);
    for(int i=0;i<workload_size;i++){
        v[i]=randint<0,10>();
    }
    // serial sum
    time_point time_start=now();
    int sum_of_v=0;
    for(int i=0;i<workload_size;i++){
        sum_of_v+=v[i];
    }
    int time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"forloop sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";
    
    // std::accumulate sum
    time_start=now();
    sum_of_v=std::accumulate(v.begin(), v.end(), 0);
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"std::accumulate sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";

    // tbb::parallel_reduce sum
    time_start=now();
    sum_of_v=tbb_parallel_sum<5000>(v.data(), v.size());
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"tbb sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";

    // std::async(fork/join) sum 
    time_start=now();
    sum_of_v=std_parallel_sum<5000>(v.begin(), v.end());
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"std::async sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";

    // tbb(fork/join) sum
    time_start=now();
    sum_of_v=tbb_fjtask_sum<5000>(v.data(), v.size());
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"tbb fork/join sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";

    // libasync init scheduler
    WorkStealingScheduler scheduler;
    register_scheduler(scheduler);

    // libasync(fork/join) sum
    time_start=now();
    sum_of_v=libasync_sum<5000>(v.begin(), v.end());
    time_elapsed=ms_elapsed_count(time_start);
    std::cout<<"libasync sum="<<sum_of_v<<", elapsed="<<time_elapsed<<"ms\n";
}