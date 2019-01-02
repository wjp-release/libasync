#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp::cf;

struct A : public Task{ 
	A(const std::string& str):x(str){}
	std::string x;
	Task* execute() override{
		std::cout<<x<<"\n";
		return nullptr;
	}
};

class CFTaskBlockTOT : public ::testing::Test {
protected:
	CFTaskBlockTOT() {}
	virtual ~CFTaskBlockTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};


TEST_F(CFTaskBlockTOT, duh) {
	TaskBlock block;
	block.emplaceTask<A>("haha!");
	A* a=block.taskPointer<A>();
	Task* t=block.taskPointer();
	EXPECT_EQ((void*)a,(void*)t);
	TaskHeader* ah=&(a->taskHeader());
	TaskHeader* bh=&(block.taskHeader());
	EXPECT_EQ(ah, bh);
	EXPECT_EQ((void*)bh, (void*)&block);
	TaskBlock* tb=bh->taskBlockPointer();
	EXPECT_EQ(tb, &block);
}



