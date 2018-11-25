#include "gtest/gtest.h"
#include "Common.h"
#include "Internal.h"

using namespace wjp;

class SpawnSyncTaskTOT : public ::testing::Test {
protected:
	SpawnSyncTaskTOT() {

	}
	virtual ~SpawnSyncTaskTOT() {

	}
	virtual void SetUp() {


	}
	virtual void TearDown() {

	}
};

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

TEST_F(SpawnSyncTaskTOT, CreateAndCall) {
	Scheduler sched;
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto t = sched.create_task<int>();
	t->bind(bee{}, 1,2,3);
	auto x= t->call();
	EXPECT_EQ(x,123);
}


TEST_F(SpawnSyncTaskTOT, SSS) {

}

