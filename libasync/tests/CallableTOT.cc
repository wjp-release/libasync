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

TEST_F(CallableTOT, Concept) {
	// Callable is invocable
	EXPECT_EQ(std::is_invocable<Callable<A>>{},1);
	// Callable is copy assignable
	EXPECT_EQ(std::is_copy_assignable<Callable<A>>{},1);
	// Callable is move assignable
	EXPECT_EQ(std::is_move_assignable<Callable<A>>{},1);
	// Callable is copy assignable
	EXPECT_EQ(std::is_copy_constructible<Callable<A>>{},1);
	// Callable is move assignable
	EXPECT_EQ(std::is_move_constructible<Callable<A>>{},1);

	

}

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
	EXPECT_EQ(c1.call().x,c0.call().x);
	c0=nullptr;
	EXPECT_FALSE((bool)c0);
	Callable<A> c4;
	c4.bind([]{
		return A("123456");
	});
	Callable<A> c2=std::ref(c1); 
	Callable<A> c3=c1; //copy constructor
	EXPECT_EQ(c1.call().x,c2.call().x);
	EXPECT_EQ(c1.call().x,c3.call().x);
	EXPECT_EQ(c1.call().x,c4.call().x);
	c4.bind([]{
		return A("123");
	});
	EXPECT_EQ(c4.call().x, "123");
	c4=c3; // copy assignment
	EXPECT_EQ(c4.call().x, "123456");
	c4.bind(c3); // bind a Callable
	EXPECT_EQ(c4().x, "123456");
	auto c5=c4;  // copy constructor
	EXPECT_EQ(c5().x, "123456");
	auto c6=Callable<A>{[]{   // move constructor & templated invocable constructor
		return A("wjpjw");
	}};
	EXPECT_EQ(c6().x, "wjpjw"); 

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
	Callable<int> c(bee{}, 1,2,3);
	EXPECT_EQ(c(),123);
}

TEST_F(CallableTOT, Simplistic) {
	auto x=Callable<int>{[](int x){return x;}, 1}();
	EXPECT_EQ(x, 1);
}



