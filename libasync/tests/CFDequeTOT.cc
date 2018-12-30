#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp::cf;

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

class CFDequeTOT : public ::testing::Test {
protected:
	CFDequeTOT() {}
	virtual ~CFDequeTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};


TEST_F(CFDequeTOT, duh) {

}



