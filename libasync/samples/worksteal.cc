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
        scheduler.submit(w);
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
        scheduler.submit(w);
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
        w->join(); // Tasks are run in completely synchronous mode now.
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
// ~ Fibonacci ~
// Calculation is deliberately delayed to make worksteal pool stats observable to the periodic monitor
void worksteal7(){
    WorkStealingScheduler scheduler;
    struct Fibonacci{  
        int operator()(WorkStealingScheduler& scheduler, int x){
            sleep(1000); // one layer deeper per second
            if(x==2) return 3;
            println("fib"+std::to_string(x));
            auto c1=scheduler.create_forkjoin_task<int>();
            c1->scheduler_aware_bind(Fibonacci{},x-1);
            c1->fork(); // c1 should be directly pushed into deque
            int result=c1->join().value_or(0);
            return x+result;
        }
    };
    auto w = scheduler.create_forkjoin_task<int>();
    w->scheduler_aware_bind(Fibonacci{},12);
    w->fork(); // external fork
    int result=w->join().value_or(0);
    println("Fibonacci(12) result="+std::to_string(result));
}

void worksteal(){
    worksteal7();
}



