#include "gtest/gtest.h"
#include "Internal.h"
#include "ConcurrentPrint.h"
using namespace wjp;

class CancelTaskTOT : public ::testing::Test {
protected:
	CancelTaskTOT() {

	}
	virtual ~CancelTaskTOT() {

	}
	virtual void SetUp() {
		scheduler=new WorkStealingScheduler();

	}
	virtual void TearDown() {
		delete scheduler;
	}
	WorkStealingScheduler* scheduler;
};

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

TEST_F(CancelTaskTOT, cancel_futuristic_task) {
	struct bee{
		int operator()(int x, int y, int z){
            println("bee!");
			return x*100+y*10+z;
		}
	};
    for(int i=0;i<100;i++){
        auto w = scheduler->create_futuristic_task<int>();
    	w->bind(bee{}, 1,2,3);
        w->submit();
        w->cancel();
    }
    sleep(100);
}

TEST_F(CancelTaskTOT, cancel_forkjoin_task) {
	struct bee{
		int operator()(int x, int y, int z){
            println("bee!");
			return x*100+y*10+z;
		}
	};
    for(int i=0;i<100;i++){
        auto w = scheduler->create_forkjoin_task<int>();
    	w->bind(bee{}, 1,2,3);
        w->fork();
        w->cancel();
    }
    sleep(100);
}

