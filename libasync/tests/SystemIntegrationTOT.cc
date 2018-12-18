#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp;
class SystemIntegration : public ::testing::Test {
protected:
	SystemIntegration() {

	}
	virtual ~SystemIntegration() {

	}
	virtual void SetUp() {


	}
	virtual void TearDown() {

	}
};


TEST_F(SystemIntegration, ExampleOfSpawnSync) {
	int i = 1;
	EXPECT_EQ(1, i);
}


