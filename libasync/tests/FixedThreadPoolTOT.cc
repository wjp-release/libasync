#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp;
class FixedThreadPoolTOT : public ::testing::Test {
protected:
	FixedThreadPoolTOT() {

	}
	virtual ~FixedThreadPoolTOT() {

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

TEST_F(FixedThreadPoolTOT, Simulation) {
	std::vector<int> workers{11,22,33,44,55,66}; //Uses int as thread-local handle
	FixedThreadPool<int>* pool= new FixedThreadPool<int>(workers,[](FixedThreadPool<int>&p, int i){
		//std::cout<<p.current_thread_index().value_or(9999)<<std::endl;
	});
	EXPECT_EQ(pool->nr_threads(),6);
	delete pool;
}


