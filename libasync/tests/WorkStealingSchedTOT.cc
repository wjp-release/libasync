#include "gtest/gtest.h"
#include "Common.h"
#include "Internal.h"

using namespace wjp;
using namespace wjp::WorkStealing;
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
	Scheduler sched;
}


TEST_F(WorkStealingSchedTOT, SSS) {

}

