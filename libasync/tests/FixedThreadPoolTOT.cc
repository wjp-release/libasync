#include "gtest/gtest.h"
#include "Common.h"
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

TEST_F(FixedThreadPoolTOT, CreateAndDelete) {
	A a("123");
	FixedThreadPool* pool= new FixedThreadPool(4,[](FixedThreadPool&p,A a){
		for(int i=0;i<10;i++){
			//std::cout<<i<<": "<<a.x<<", tid="<<std::this_thread::get_id()<<std::endl;
			//sleep(300);
			//std::cout<<"in thread: "<<p.contains_me()<<std::endl;
		}
	}, a);
	EXPECT_EQ(4, pool->nr_threads());
	delete pool;
}


TEST_F(FixedThreadPoolTOT, Nr) {
	A a("123");
	FixedThreadPool* pool= new FixedThreadPool(4,[](FixedThreadPool&p){
		//std::cout<<"in thread: "<<p.contains_me()<<std::endl;
	});
	//std::cout<<pool->nr_threads();
	EXPECT_FALSE(pool->contains_me());
	delete pool;
}

TEST_F(FixedThreadPoolTOT, Vector) {
	A a("123");
	std::vector<int> workers{11,22,33,44,55,66};
	FixedThreadPool* pool= new FixedThreadPool(workers,[](FixedThreadPool&p, int i){
		std::cout<<"thread: "<<i<<std::endl;
	});
	std::cout<<pool->nr_threads();
	delete pool;
}


