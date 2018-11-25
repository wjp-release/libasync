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
	WorkStealingScheduler sched;
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


TEST_F(SpawnSyncTaskTOT, Assignment) {
	WorkStealingScheduler sched;
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto w = sched.create_task<int>();
	w->bind(bee{}, 1,2,3);
	auto t = sched.create_task<int>();
	t->bind([](int x, int y){
				return x+y;
			},10,20);
	auto x=t->call();
	EXPECT_EQ(x, 30);
	WorkStealingScheduler::Task copy=*t; //copy construct from t
	EXPECT_EQ(copy(), 30);
	copy=nullptr; // inherited operator= nullptr
	EXPECT_FALSE((bool)copy); // inherited operator bool
	copy=*w; //assignment operator
	EXPECT_EQ(copy(), 123);
	
}

