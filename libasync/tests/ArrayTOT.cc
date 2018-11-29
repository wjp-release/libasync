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
    struct Bobo{
        Bobo(int n) : threads(n), workers(n)
        {
            for(int i=0;i<n;i++){
                workers.emplace_back("worker"+std::to_string(i));
            }
            for(int i=0;i<n;i++){
                threads.emplace_back([]{
                    // std::cout<<"bobo"<<this->workers[i].x<<"\n";
                    //std::cout<<"bobo"<<i<<"\n";
                });
            }
        }
        ~Bobo(){
            for(auto& t:threads){
                t.join();
            }
        }
        Array<std::thread> threads;
        Array<A> workers;
    };
	Bobo bobo(9);
	EXPECT_EQ(bobo.workers.capacity(),9);
}


