#include "gtest/gtest.h"
#include "Internal.h"
#include "Async.h"
#include <functional>

using namespace wjp;

class AsyncTOT : public ::testing::Test {
protected:
	AsyncTOT() {}
	virtual ~AsyncTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

TEST_F(AsyncTOT, register_get) {
    WorkStealingScheduler scheduler;
    register_scheduler(scheduler);
    EXPECT_EQ(&scheduler, &get_scheduler());
}

TEST_F(AsyncTOT, sched_aware_async) {
    WorkStealingScheduler scheduler;
    struct SumTree{  
        int operator()(WorkStealingScheduler& scheduler, int x){
            if(x==2) return 3;
            auto subtask=SumTree{}|sched_aware_async<int>(scheduler, x-1);
            return x+subtask->join();
        }
    };
    auto w = SumTree{}|sched_aware_async<int>(scheduler, 12);
    EXPECT_EQ(w->join(), 78);
}


TEST_F(AsyncTOT, functor_async) {
    WorkStealingScheduler scheduler;
    register_scheduler(scheduler);
    struct SumTree{  
        int operator()(int x){
            if(x==2) return 3;
            auto subtask=SumTree{}|async<int>(x-1);
            return x+subtask->join();
        }
    };
    auto w = SumTree{}|async<int>(12);
    EXPECT_EQ(w->join(), 78);
}

TEST_F(AsyncTOT, lambda_async) {
    WorkStealingScheduler scheduler;
    register_scheduler(scheduler);
    std::function<int(int)> sum_lambda=[&sum_lambda](int x){
        if(x==2) return 3;
        auto y=sum_lambda|async<int>(x-1);
        return x+y->join();
    };
    auto w = sum_lambda|async<int>(12);
    EXPECT_EQ(w->join(), 78);
}

static int sum(int x){
    if(x==2) return 3;
    auto y=sum|async<int>(x-1);
    return x+y->join();
}

TEST_F(AsyncTOT, function_async) {
    WorkStealingScheduler scheduler;
    register_scheduler(scheduler);
    auto w = sum|async<int>(12);
    EXPECT_EQ(w->join(), 78);
}