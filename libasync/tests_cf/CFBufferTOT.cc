#include "gtest/gtest.h"
#include "CFBuffer.h"
#include "CFTask.h"
#include "CFTaskHeader.h"
#include <iostream>
#include <string>

using namespace wjp::cf;

struct StringTask : public Task{ 
public:
	StringTask(const std::string& str):x(str){}
	std::string x;
protected:
	Task* compute() override{
		std::cout<<x<<"\n";
		return nullptr;
	}
};


class CFBufferTOT : public ::testing::Test {
protected:
	CFBufferTOT() {}
	virtual ~CFBufferTOT() {}
	virtual void SetUp() {
		buffer=new TaskBuffer();
	}
	virtual void TearDown() {
		delete buffer;
	}
	TaskBuffer* buffer;
};

TEST_F(CFBufferTOT, EmplaceSteal) {
	EXPECT_EQ(buffer->size(), 0);
	// emplace
	auto t1=buffer->emplace<StringTask>("t1");
	auto t2=buffer->emplace<StringTask>("t2");
	auto t3=buffer->emplace<StringTask>("t3");
	auto t4=buffer->emplace<StringTask>("t3");
	EXPECT_EQ(t1->taskHeader().state, TaskHeader::Ready);
	EXPECT_EQ(t2->taskHeader().state, TaskHeader::Ready);
	EXPECT_EQ(t3->taskHeader().state, TaskHeader::Ready);
	EXPECT_EQ(t4->taskHeader().state, TaskHeader::Ready);
	EXPECT_EQ(buffer->size(), 4);
	// steal
	auto s1=buffer->steal();
	auto s2=buffer->steal();
	auto s3=buffer->steal();
	auto s4=buffer->steal();
	EXPECT_EQ(s1->taskHeader().state, TaskHeader::StolenFromBuffer);
	EXPECT_EQ(s2->taskHeader().state, TaskHeader::StolenFromBuffer);
	EXPECT_EQ(s3->taskHeader().state, TaskHeader::StolenFromBuffer);
	EXPECT_EQ(s4->taskHeader().state, TaskHeader::StolenFromBuffer);
	EXPECT_EQ(s1, (Task*)t1);
	EXPECT_EQ(s2, (Task*)t2);
	EXPECT_EQ(s3, (Task*)t3);
	EXPECT_EQ(s4, (Task*)t4);
	EXPECT_EQ(buffer->size(), 0);
	auto s5=buffer->steal();
	EXPECT_EQ(s5, nullptr);
}


TEST_F(CFBufferTOT, StubExecAndReclaim) {

}


