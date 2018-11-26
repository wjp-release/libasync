#include "gtest/gtest.h"
#include "Common.h"
#include "Internal.h"

using namespace wjp;

class SpawnSyncTaskTOT : public ::testing::Test {
protected:
	SpawnSyncTaskTOT() {

	}
	virtual ~SpawnSyncTaskTOT() {

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

TEST_F(SpawnSyncTaskTOT, CreateAndCall) {

}

