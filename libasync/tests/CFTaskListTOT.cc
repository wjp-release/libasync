#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp::cf;

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

class CFTaskListTOT : public ::testing::Test {
protected:
	CFTaskListTOT() {}
	virtual ~CFTaskListTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};


TEST_F(CFTaskListTOT, pushBackPopBack) {
	TaskBlock blocks[1024];
	TaskList list;
	for(auto& block : blocks){
		list.pushBack(block.taskPointer());
	}

	for(int i=0;i<1024;i++){
		Task* t=list.popBack();
		TaskBlock* block=t->taskHeader().taskBlockPointer();
		EXPECT_EQ(block, &blocks[1023-i]);
	}
}



