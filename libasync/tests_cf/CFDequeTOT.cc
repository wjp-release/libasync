#include "gtest/gtest.h"
#include "CFDeque.h"
#include "CFTask.h"
#include "CFTaskHeader.h"

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


class CFDequeTOT : public ::testing::Test {
protected:
	CFDequeTOT() {}
	virtual ~CFDequeTOT() {}
	virtual void SetUp() {
		deque=new TaskDeque();
	}
	virtual void TearDown() {
		delete deque;
	}
	TaskDeque* deque;
};

TEST_F(CFDequeTOT, Construct) {
	EXPECT_EQ(deque->freeListSize(), DequeCapacity);
	EXPECT_EQ(deque->stolenListSize(), 0);
	EXPECT_EQ(deque->readyListSize(), 0);
	EXPECT_EQ(deque->execListSize(), 0);
	EXPECT_EQ(deque->NumberOfEmplacedTasks(), 0);
}


TEST_F(CFDequeTOT, EmpalceStealTake) {
	// emplace
	auto t1=deque->emplace<StringTask>("t1");
	EXPECT_EQ(t1->taskHeader().state, TaskHeader::Ready);
	auto t2=deque->emplace<StringTask>("t2");
	auto t3=deque->emplace<StringTask>("t3");
	auto t4=deque->emplace<StringTask>("t4");
	EXPECT_EQ(deque->NumberOfEmplacedTasks(), 4);
	EXPECT_EQ(deque->readyListSize(), 4);
	EXPECT_EQ(deque->execListSize(), 0);

	// steal 
	Task* steal1=deque->steal();
	EXPECT_EQ(steal1, (Task*)t1); // steal oldest (coldest cache)
	EXPECT_EQ(steal1->taskHeader().state, TaskHeader::Stolen);
	Task* steal2=deque->steal();
	EXPECT_EQ(steal2, (Task*)t2);
	EXPECT_EQ(steal2->taskHeader().state, TaskHeader::Stolen);
	EXPECT_EQ(deque->NumberOfEmplacedTasks(), 4);
	EXPECT_EQ(deque->stolenListSize(), 2);
	EXPECT_EQ(deque->readyListSize(), 2);
	EXPECT_EQ(deque->execListSize(), 0);

	// take
	Task* take1=deque->take();
	EXPECT_EQ(take1, (Task*)t4); // take newest (hottest cache)
	EXPECT_EQ(take1->taskHeader().state, TaskHeader::Exec);
	Task* take2=deque->take();
	EXPECT_EQ(take2, (Task*)t3);
	EXPECT_EQ(take2->taskHeader().state, TaskHeader::Exec);
	EXPECT_EQ(deque->stolenListSize(), 2);
	EXPECT_EQ(deque->readyListSize(), 0);
	EXPECT_EQ(deque->execListSize(), 2);
	EXPECT_EQ(deque->NumberOfEmplacedTasks(), 4);

	// continue to take when empty
	Task* take3=deque->take();
	EXPECT_EQ(take3, nullptr);
	
	// continue to steal when empty
	Task* steal3=deque->steal();
	EXPECT_EQ(steal3, nullptr);
}


