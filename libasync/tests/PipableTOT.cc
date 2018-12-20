#include "gtest/gtest.h"
#include "Internal.h"
#include "Pipable.h" 

using namespace wjp;

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

class PipableTOT : public ::testing::Test {
protected:
	PipableTOT() {}
	virtual ~PipableTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};


TEST_F(PipableTOT, Sum) {
	int duh = 1 | sum(2) | sum(3);  // sum(2) is a pipe_closure
	EXPECT_EQ(duh, 6);
}


TEST_F(PipableTOT, AddOne) {
	int dyh = 1 | add_one | add_one; // add_one is a pipe_closure
	EXPECT_EQ(dyh, 3);
}


