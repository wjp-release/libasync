#include "gtest/gtest.h"
#include "CFDeque.h"
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


class CFIntegratedTOT : public ::testing::Test {
protected:
	CFIntegratedTOT() {}
	virtual ~CFIntegratedTOT() {}
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
};

TEST_F(CFIntegratedTOT, Construct) {
	
}


TEST_F(CFIntegratedTOT, Empalce) {

}


