#include "gtest/gtest.h"
#include "Common.h"
#include "Internal.h"

using namespace wjp;

class WorkStealingSchedulerTOT : public ::testing::Test {
protected:
	WorkStealingSchedulerTOT() {

	}
	virtual ~WorkStealingSchedulerTOT() {

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

TEST_F(WorkStealingSchedulerTOT, SS) {

}


TEST_F(WorkStealingSchedulerTOT, SSS) {

}

