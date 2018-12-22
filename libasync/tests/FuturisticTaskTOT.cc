#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp;

// This testsuit executes FuturisticTask simply in a thread
// to avoid dependency on the complicated work-stealing scheduler/worker/pool.

// You must call ready_for_sched before asynchronous execution though, 
// otherwise libasync assumes you intends to execute the freelance task in your current thread. 

class FuturisticTaskTOT : public ::testing::Test {
protected:
	FuturisticTaskTOT() {

	}
	virtual ~FuturisticTaskTOT() {

	}
	virtual void SetUp() {
		global=new WorkStealingScheduler();

	}
	virtual void TearDown() {
		delete global;
	}
	WorkStealingScheduler* global;
};

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

TEST_F(FuturisticTaskTOT, CreateAndCall) {
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto t = global->create_futuristic_task<int>();
	t->bind(bee{}, 1,2,3);
	auto x= t->call();
	EXPECT_EQ(x,123);
}


TEST_F(FuturisticTaskTOT, Assignment) {
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto w = global->create_futuristic_task<int>();
	w->bind(bee{}, 1,2,3);
	auto t = global->create_futuristic_task<int>();
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
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto w = global->create_futuristic_task<int>();
	w->bind(bee{}, 1,2,3);
	w->ready_for_sched();
	std::thread([=]{
		sleep(30);
		w->execute();
	}).detach();
	w->wait();
	EXPECT_TRUE(w->is_finished());
}


TEST_F(FuturisticTaskTOT, SimulationGetSuccess) {
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	auto w = global->create_futuristic_task<int>();
	w->bind(bee{}, 1,2,3);
	w->ready_for_sched();
	std::thread([=]{
		sleep(30);
		w->execute();
	}).detach();
	auto res=w->get_quietly();
	int x=res.value_or(0);
	EXPECT_EQ(x, 123);
}


TEST_F(FuturisticTaskTOT, SimulationGetFailure) {
	struct bee{
		int operator()(int x, int y, int z){
			if(x==1) throw std::runtime_error("damn x==1!");
			return x*100+y*10+z;
		}
	};
	auto w = global->create_futuristic_task<int>();
	w->bind(bee{}, 1,2,3);
	w->ready_for_sched();
	std::thread([=]{
		w->execute(); // will throw "damn x==1!"
	}).detach();
	try{
		w->get();
	}catch(std::exception& e){
		EXPECT_EQ(std::string(e.what()), "damn x==1!");
	}
}
