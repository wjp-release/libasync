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
            while(true){ // Note that sleep(200) doesn't work properly here.
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
            while(true){ // Note that sleep(200) doesn't work properly here.
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

void worksteal(){
    worksteal1();
}



