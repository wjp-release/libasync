#include "sample.h"
#include "Internal.h"

using namespace wjp;

// Submit a task every 100 ms. It takes 2000 ms for each task to complete. 
// There will be pending tasks after 900ms. It's very unlikely for workers to be idle then.
void worksteal1(){
    WorkStealingScheduler scheduler;
    struct bee{
        int operator()(int x, int y, int z){
            time_point start=now();
            while(true){ 
                if(ms_elapsed_count(start)>2000) break; 
            }
            return x*100+y*10+z;
        }
    };
    for(int i=0;i<50;i++){
        auto w = scheduler.create_futuristic_task<int>();
        w->bind(bee{}, 1,2,3);
        w->submit();
        sleep(100);
    }
}

// Submit a task every 100 ms. It takes 200 ms for each task to complete.
// Since we have 9 workers (on 4-cpu platforms), workers should almost always have empty buffers.
void worksteal2(){
    WorkStealingScheduler scheduler;
    struct bee{
        int operator()(int x, int y, int z){
            time_point start=now();
            while(true){ 
                if(ms_elapsed_count(start)>200) break; 
            }
            return x*100+y*10+z;
        }
    };
    for(int i=0;i<50;i++){
        auto w = scheduler.create_futuristic_task<int>();
        w->bind(bee{}, 1,2,3);
        w->submit();
        sleep(100);
    }
}

// Fork/Join: 
// User-supplied function now has access to the reference to the scheduler.
void worksteal3(){
    WorkStealingScheduler scheduler;
    struct bee{
        int operator()(WorkStealingScheduler& scheduler, int x, int y, int z){
            time_point start=now();
            int nr_workers=scheduler.nr_workers();
            println("bee:"+std::to_string(nr_workers));
            while(true){ 
                if(ms_elapsed_count(start)>2000) break; 
            }
            return x*100+y*10+z;
        }
    };
    for(int i=0;i<50;i++){
        auto w = scheduler.create_forkjoin_task<int>();
        w->scheduler_aware_bind(bee{},1,2,3);
        w->fork();
        sleep(100);
    }
}

// Fork/Join
// join() as the means of synchronization. 
void worksteal4(){
    WorkStealingScheduler scheduler;
    struct bee{
        int operator()(WorkStealingScheduler& scheduler, int x, int y, int z){
            time_point start=now();
            while(true){ 
                if(ms_elapsed_count(start)>2000) break; 
            }
            return x*100+y*10+z;
        }
    };
    for(int i=0;i<7;i++){
        auto w = scheduler.create_forkjoin_task<int>();
        w->scheduler_aware_bind(bee{},1,2,3);
        w->fork();
        w->join_quietly(); // Tasks are run in completely synchronous mode now.
        int result=w->get_quietly().value_or(0); // expect 123  
        println("Task"+std::to_string(i)+" result="+std::to_string(result));
    }
}

// Fork/Join
// try sleep() instead of busy waiting
void worksteal5(){
    WorkStealingScheduler scheduler;
    struct bee{
        int operator()(WorkStealingScheduler& scheduler, int x, int y, int z){
            sleep(2000);
            // sleep() is actually more accurate than busy waiting 
            // according to test results
            return x*100+y*10+z;
        }
    };
    for(int i=0;i<50;i++){
        auto w = scheduler.create_forkjoin_task<int>();
        w->scheduler_aware_bind(bee{},1,2,3);
        w->fork();
        sleep(100);
    }
}


// Fork/Join
// spawn children now
// For each w task, 10 time-consuming c1 tasks should be pushed into deque.
// Monitor is expected to capture the side-effects of the deque flush and subsequent stealing process.
// Since child tasks have higher priority, externally forked tasks are expected to stay in submission buffer.
void worksteal6(){
    WorkStealingScheduler scheduler;
    struct bee{
        int operator()(WorkStealingScheduler& scheduler, int x, int y, int z){
            sleep(2000);
            for(int i=0;i<10;i++){
                auto c1=scheduler.create_forkjoin_task<int>();
                c1->scheduler_aware_bind([](WorkStealingScheduler& scheduler, int x){
                    println("c1->"+std::to_string(x));
                    sleep(500);
                    return x;
                }, 1000);
                c1->fork(); // c1 should be directly pushed into deque
            }
            return x*100+y*10+z;
        }
    };
    for(int i=0;i<50;i++){
        auto w = scheduler.create_forkjoin_task<int>();
        w->scheduler_aware_bind(bee{},1,2,3);
        w->fork(); // external fork
        sleep(100);
    }
}


// Fork/Join
// ~ SumTree ~
// Each task spawns one child task
void worksteal7(){
    WorkStealingScheduler scheduler;
    struct SumTree{  
        int operator()(WorkStealingScheduler& scheduler, int x){
            // Calculation is deliberately delayed to make worksteal pool stats observable to the periodic monitor
            sleep(1000); 
            if(x==2) return 3;
            println("sum"+std::to_string(x));
            auto c1=scheduler.create_forkjoin_task<int>();
            c1->scheduler_aware_bind(SumTree{},x-1);
            c1->fork(); // c1 should be directly pushed into deque
            int result=c1->join_quietly().value_or(0);
            return x+result;
        }
    };
    auto w = scheduler.create_forkjoin_task<int>();
    w->scheduler_aware_bind(SumTree{},12);
    w->fork(); // external fork
    int result=w->join_quietly().value_or(0);
    println("SumTree(12) result="+std::to_string(result));
}


// Fork/Join
// ~ Fibonacci Example ~
// Each task spawns 2 child tasks. 
void worksteal8(){
    WorkStealingScheduler scheduler;
    struct Fibonacci{  
        int operator()(WorkStealingScheduler& scheduler, int x){
            if(x==1) return 1;
            if(x==2) return 1;
            println("fib"+std::to_string(x));
            auto c1=scheduler.create_forkjoin_task<int>();
            c1->scheduler_aware_bind(Fibonacci{},x-1);
            c1->fork(); 
            auto c2=scheduler.create_forkjoin_task<int>();
            c2->scheduler_aware_bind(Fibonacci{},x-2);
            c2->fork(); 
            int v1=c1->join_quietly().value_or(0);
            int v2=c2->join_quietly().value_or(0);
            return v1+v2;
        }
    };
    auto w = scheduler.create_forkjoin_task<int>();
    w->scheduler_aware_bind(Fibonacci{},12);
    w->fork(); // external fork
    int result=w->join_quietly().value_or(0);
    println("Fibonacci(12) result="+std::to_string(result));
}

template <typename RandomIt>
int std_ps(RandomIt beg, RandomIt end)
{
    auto len = end - beg;
    if (len < 1000) return std::accumulate(beg, end, 0);
    RandomIt mid = beg + len/2;
    auto handle = std::async(std::launch::async, std_ps<RandomIt>, mid, end);
    int sum = std_ps(beg, mid);
    return sum + handle.get();
}

// parallel_sum
// https://en.cppreference.com/w/cpp/thread/async
template <typename RandomIt>
int parallel_sum(WorkStealingScheduler& scheduler, RandomIt beg, RandomIt end)
{
    auto len = end - beg;
    if (len < 1000) return std::accumulate(beg, end, 0);
    RandomIt mid = beg + len/2;

    auto w = scheduler.create_forkjoin_task<int>();  
    w->scheduler_aware_bind(parallel_sum<RandomIt>, mid, end);
    w->fork(); 

    int sum = parallel_sum(scheduler, beg, mid);
    return sum + w->join_quietly().value();
}

// Fork/Join
// Parallel Sum
#define workload_size 10000000
void worksteal9()
{
    // init workload 
    std::vector<int> v(workload_size);
    for(int i=0;i<workload_size;i++){
        v[i]=randint<0,10>();
    }
    // serial sum
    time_point serial_start=now();
    int serial_sum=0;
    for(int i=0;i<workload_size;i++){
        serial_sum+=v[i];
    }
    int serial_elapsed=ms_elapsed_count(serial_start);
    println("forloop sum="+std::to_string(serial_sum)+", elapsed="+std::to_string(serial_elapsed)+"ms");
    // std::accumulate
    serial_start=now();
    serial_sum=std::accumulate(v.begin(), v.end(), 0);
    serial_elapsed=ms_elapsed_count(serial_start);
    println("std::accumulate sum="+std::to_string(serial_sum)+", elapsed="+std::to_string(serial_elapsed)+"ms");

    // parallel sum
    WorkStealingScheduler scheduler; 
    time_point start=now();
    int sum=parallel_sum(scheduler, v.begin(), v.end());
    int elapsed=ms_elapsed_count(start);
    println("worksteal sum="+std::to_string(sum)+", elapsed="+std::to_string(elapsed)+"ms");
#ifdef INTERNAL_STATS
    std::cout<<"nr_tasks="<<scheduler.nr_tasks_executed<<std::endl;
#endif    
    // std::async
    time_point start2=now();
    int sum2=std_ps(v.begin(), v.end());
    int elapsed2=ms_elapsed_count(start2);
    println("async sum="+std::to_string(sum2)+", elapsed="+std::to_string(elapsed2)+"ms");
}

// Less verbose semantics 
void worksteal10(){
    WorkStealingScheduler scheduler;
    struct SumTree{  
        int operator()(WorkStealingScheduler& scheduler, int x){
            if(x==2) return 3;
            return x+scheduler.spawn<int>(SumTree{},x-1)->join();
        }
    };
    auto w = scheduler.spawn<int>(SumTree{},12);
    println("SumTree(12) result="+std::to_string(w->join()));
}

// Pipable semantics
void worksteal11(){
    WorkStealingScheduler scheduler;
}

void worksteal(){
    worksteal11();
}



