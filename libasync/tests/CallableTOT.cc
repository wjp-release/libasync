#include "gtest/gtest.h"
#include "Internal.h"

using namespace wjp;

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

class CallableTOT : public ::testing::Test {
protected:
	CallableTOT() {}
	virtual ~CallableTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};


TEST_F(CallableTOT, CopyAssignInitMove) {
	Callable<A> c0;
	Callable<A> c1;
	c1.bind(
		[](std::string x){
			return A("123"+x);
		}, 
		"456"
	);
	c0=c1;
	EXPECT_EQ(c1().x,c0().x);
	c0=nullptr;
	EXPECT_FALSE((bool)c0);
	Callable<A> c4;
	c4.bind([]{
		return A("123456");
	});
	Callable<A> c2=std::ref(c1); 
	Callable<A> c3=c1;
	EXPECT_EQ(c1().x,c2().x);
	EXPECT_EQ(c1().x,c3().x);
	EXPECT_EQ(c1().x,c4().x);
	c4.bind([]{
		return A("123");
	});
	EXPECT_EQ(c4().x, "123");
	c4=c3;
	EXPECT_EQ(c4().x, "123456");
}



TEST_F(CallableTOT, Lambda) {
	Callable<A> callable;
	callable.bind (
		[](std::string x){
			return A("123"+x);
		}, 
		"456"
	);
	auto x= callable();
	EXPECT_EQ(x.x,"123456");
}


TEST_F(CallableTOT, Functor) {
	struct duh{
		int operator()(){
			return 333;
		}
	};
	Callable<int> callable;
	callable.bind (duh{});
	auto x= callable();
	EXPECT_EQ(x,333);
}


TEST_F(CallableTOT, MultiArgs) {
	struct bee{
		int operator()(int x, int y, int z){
			return x*100+y*10+z;
		}
	};
	Callable<int> callable;
	callable.bind(bee{}, 1,2,3);
	auto x= callable();
	EXPECT_EQ(x,123);
}

