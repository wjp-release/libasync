#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp;

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

class UtilitiesTOT : public ::testing::Test {
protected:
	UtilitiesTOT() {}
	virtual ~UtilitiesTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};


TEST_F(UtilitiesTOT, SS) {


}




TEST_F(UtilitiesTOT, SSS) {

}


