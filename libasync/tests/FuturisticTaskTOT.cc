#include "gtest/gtest.h"
#include "Common.h"
#include "Internal.h"

using namespace wjp;

class FuturisticTaskTOT : public ::testing::Test {
protected:
	FuturisticTaskTOT() {

	}
	virtual ~FuturisticTaskTOT() {

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

TEST_F(FuturisticTaskTOT, CreateAndCall) {
	WorkStealingScheduler sched;
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto t = sched.create_futuristic_task<int>();
	t->bind(bee{}, 1,2,3);
	auto x= t->call();
	EXPECT_EQ(x,123);
}


TEST_F(FuturisticTaskTOT, Assignment) {
	WorkStealingScheduler sched;
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto w = sched.create_futuristic_task<int>();
	w->bind(bee{}, 1,2,3);
	auto t = sched.create_futuristic_task<int>();
	t->bind([](int x, int y){
				return x+y;
			},10,20);
	auto x=t->call();
	EXPECT_EQ(x, 30);
	WorkStealingScheduler::FuturisticTask copy=*t; //copy construct from t
	EXPECT_EQ(copy(), 30);
	copy=nullptr; // inherited operator= nullptr
	EXPECT_FALSE((bool)copy); // inherited operator bool
	copy=*w; //assignment operator
	EXPECT_EQ(copy(), 123);
	
}


TEST_F(FuturisticTaskTOT, SimulationWait) {
	WorkStealingScheduler sched;
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto w = sched.create_futuristic_task<int>();
	w->bind(bee{}, 1,2,3);
	std::thread([=]{
		sleep(300);
		w->execute();
	}).detach();
	w->wait();
	EXPECT_TRUE(w->is_finished());
}


TEST_F(FuturisticTaskTOT, SimulationGetSuccess) {
	WorkStealingScheduler sched;
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto w = sched.create_futuristic_task<int>();
	w->bind(bee{}, 1,2,3);
	std::thread([=]{
		w->execute();
	}).detach();
	auto res=w->get_quietly();
	int x=res.value_or(0);
	EXPECT_EQ(x, 123);
}


TEST_F(FuturisticTaskTOT, SimulationGetFailure) {
	WorkStealingScheduler sched;
	struct bee{
		int operator()(int x, int y, int z){
			if(x==1) throw std::runtime_error("damn x==1!");
			return x*100+y*10+z;
		}
	};
	auto w = sched.create_futuristic_task<int>();
	w->bind(bee{}, 1,2,3);
	std::thread([=]{
		w->execute(); // will throw "damn x==1!"
	}).detach();
	try{
		w->get();
	}catch(std::exception& e){
		EXPECT_EQ(std::string(e.what()), "damn x==1!");
	}
}
