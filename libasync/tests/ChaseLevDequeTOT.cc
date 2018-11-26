#include "gtest/gtest.h"
#include "Internal.h"
#include "ChaseLevDeque.h"
#include <thread>
#include <set>
#include <mutex>

using namespace wjp;

struct A{
	A(const std::string& str):x(str){}
	std::string x;
};

class ChaseLevDequeTOT : public ::testing::Test {
protected:
	ChaseLevDequeTOT() {}
	virtual ~ChaseLevDequeTOT() {}
	virtual void SetUp() {
		q=new ChaseLevDeque<A>(3);
		count=0;
	}
	virtual void TearDown() {
		delete q;
	}
	ChaseLevDeque<A>* 		q;
	std::atomic<int> 		count;  // how many are stolen
};

TEST_F(ChaseLevDequeTOT, SingleThreadSituationA) {
	auto t1=std::make_shared<A>("1");
	auto t2=std::make_shared<A>("2");
	auto t3=std::make_shared<A>("3");
	auto t4=std::make_shared<A>("4");
	auto t5=std::make_shared<A>("5");
	auto t6=std::make_shared<A>("6");
	auto t7=std::make_shared<A>("7");
	auto t8=std::make_shared<A>("8");
	auto t9=std::make_shared<A>("9");
	auto t10=std::make_shared<A>("10");
	auto t11=std::make_shared<A>("11");
	auto t12=std::make_shared<A>("12");
	q->push(t1);
	q->push(t2);
	q->push(t3);
	q->push(t4);
	q->push(t5);
	q->push(t6);
	q->push(t7);
	q->push(t8);
	q->push(t9);
	q->push(t10);
	q->push(t11);
	q->push(t12);
	auto x=q->take();
	EXPECT_EQ(x->x, "12");
	x=q->take();
	EXPECT_EQ(x->x, "11");
	x=q->steal();
	EXPECT_EQ(x->x, "1");
	x=q->take();
	EXPECT_EQ(x->x, "10");
	x=q->take();
	EXPECT_EQ(x->x, "9");
	x=q->steal();
	EXPECT_EQ(x->x, "2");
	x=q->steal();
	EXPECT_EQ(x->x, "3");
	x=q->steal();
	EXPECT_EQ(x->x, "4");
	x=q->steal();
	EXPECT_EQ(x->x, "5");
	x=q->steal();
	EXPECT_EQ(x->x, "6");
	x=q->take();
	EXPECT_EQ(x->x, "8");
	x=q->steal();
	EXPECT_EQ(x->x, "7");
	x=q->steal();
	EXPECT_EQ(x, nullptr);
	x=q->take();
	EXPECT_EQ(x, nullptr);
}

TEST_F(ChaseLevDequeTOT, SingleThreadSituationInt) {
	auto qq=new ChaseLevDeque<int>(2);
	auto t1=std::make_shared<int>(1);
	auto t2=std::make_shared<int>(2);
	auto t3=std::make_shared<int>(3);
	auto t4=std::make_shared<int>(4);
	qq->push(t1);
	auto x=qq->take();
	EXPECT_EQ(*x, 1);
	qq->push(t1);
	qq->push(t2);
	qq->push(t3);
	qq->push(t4);
	x=qq->steal();
	EXPECT_EQ(*x, 1);
	x=qq->steal();
	EXPECT_EQ(*x, 2);
	x=qq->steal();
	EXPECT_EQ(*x, 3);
	x=qq->steal();
	EXPECT_EQ(*x, 4);
}

TEST_F(ChaseLevDequeTOT, StealerRace) {
	for(int i=1;i<=100;i++){
		q->push(std::make_shared<A>(std::to_string(i)));
	}	
	std::vector<std::future<std::string>> tasks;
	for(int i=0;i<10;i++){
		tasks.emplace_back(std::async(std::launch::async, [this, i]{
			int value=0;
			std::string res="thread"+std::to_string(i)+": ";
			for(int k=0;k<100;k++){
				auto x=q->steal();
				sleep(randint<1,2>());
				if(x==nullptr){
					continue;
				}
				count++;
				int tmp=std::stoi(x->x);
				res+=x->x+",";
				EXPECT_TRUE(tmp>value && tmp<=100 &&tmp>0);
			}
			return res;
		}));
	}
	for(int i=0;i<tasks.size();i++)
    {
        std::cout<<tasks[i].get()<<std::endl;
    }
	EXPECT_EQ(count, 100);
}

