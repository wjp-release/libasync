#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp;

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

class AsyncTOT : public ::testing::Test {
protected:
	AsyncTOT() {}
	virtual ~AsyncTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};


TEST_F(AsyncTOT, SS) {


}




TEST_F(AsyncTOT, SSS) {

}


