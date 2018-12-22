#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp;

class WorkStealingSchedulerTOT : public ::testing::Test {
protected:
	WorkStealingSchedulerTOT() {

	}
	virtual ~WorkStealingSchedulerTOT() {

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


TEST_F(WorkStealingSchedulerTOT, SubmitFromTask) {
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto w = scheduler->create_futuristic_task<int>();
	w->bind(bee{}, 1,2,3);
	w->submit();
	auto res=w->get_quietly();
	int x=res.value_or(9999);
	EXPECT_EQ(x, 123);
}
