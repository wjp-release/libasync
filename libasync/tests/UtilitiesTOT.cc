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


TEST_F(UtilitiesTOT, to_ms) {
	constexpr static auto ten_seconds=10s;  
	std::chrono::milliseconds tmp=to_ms(ten_seconds);
	EXPECT_EQ(tmp.count(), 10000);
}




TEST_F(UtilitiesTOT, to_sec) {
	constexpr static auto duh=3456ms;  
	auto tmp=to_sec(duh);
	EXPECT_EQ(tmp.count(), 3);
}


