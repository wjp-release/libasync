#include "gtest/gtest.h"
#include "Common.h"
#include "Internal.h"

using namespace wjp;

class WorkStealingSchedTOT : public ::testing::Test {
protected:
	WorkStealingSchedTOT() {

	}
	virtual ~WorkStealingSchedTOT() {

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

TEST_F(WorkStealingSchedTOT, SS) {

}


TEST_F(WorkStealingSchedTOT, SSS) {

}

