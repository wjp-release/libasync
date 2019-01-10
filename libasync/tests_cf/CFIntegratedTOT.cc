#include "gtest/gtest.h"
#include "CFDeque.h"
#include "CFTask.h"
#include "CFPool.h"
#include "CFTaskHeader.h"
#include <iostream>
#include <string>
#include "ConcurrentPrint.h"
#include "ThreadUtilities.h"

using namespace wjp::cf;

struct AddOne : public Task{ 
public:
	AddOne(int v):value(v){}
protected:
	int value;
	std::string stats() override{
		return Task::stats()+"("+std::to_string(value)+")";
	}
	Task* compute() override{
		if(value==14) return nullptr;
		wjp::sleep(1000);
		AddOne* child1=spawnDetached<AddOne>(value+1);
		AddOne* child2=spawnDetached<AddOne>(value+1);
		if constexpr(PrintTestTaskOutput) wjp::println(std::to_string(value));		
		return nullptr;
	}
};


class CFIntegratedTOT : public ::testing::Test {
protected:
	CFIntegratedTOT() {}
	virtual ~CFIntegratedTOT() {}
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
};

TEST_F(CFIntegratedTOT, StartTaskPool) {
	TaskPool::instance().start();	
}

TEST_F(CFIntegratedTOT, EmplaceExternallySpawnDetached) {

	
}



TEST_F(CFIntegratedTOT, EmpalceSpawnDetached) {


}


