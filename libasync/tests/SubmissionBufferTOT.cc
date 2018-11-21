#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp;

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

class SubmissionBufferTOT : public ::testing::Test {
protected:
	SubmissionBufferTOT() {}
	virtual ~SubmissionBufferTOT() {}
	virtual void SetUp() {
	}
	virtual void TearDown() {
	}
	SubmissionBuffer<A> b;
};

TEST_F(SubmissionBufferTOT, simple) {
	b.submit(std::make_shared<A>("123"));
	b.submit(std::make_shared<A>("456"));
	b.submit(std::make_shared<A>("789"));
	auto e=std::make_shared<A>("xxx");
	b.submit(e);
	auto w=b.withdraw(e);
	EXPECT_TRUE(w);
	auto x=b.steal();
	EXPECT_EQ(x->x, "123");
	x=b.steal();
	EXPECT_EQ(x->x, "456");
	x=b.steal();
	EXPECT_EQ(x->x, "789");
	x=b.steal();
	EXPECT_EQ(x, nullptr);
}


