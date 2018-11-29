#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp;

struct A{
	A(const std::string& str):x(str){}
	A(const A&)=delete;
	std::string x;
};

class ArrayTOT : public ::testing::Test {
protected:
	ArrayTOT() {}
	virtual ~ArrayTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};


TEST_F(ArrayTOT, ThreadArray) {


}


TEST_F(ArrayTOT, NonCopyableArray) {
	

}


