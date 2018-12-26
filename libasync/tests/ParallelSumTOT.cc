#include "gtest/gtest.h"
#include "PFParallelSum.h"
#include <functional>

using namespace wjp;

class ParallelSumTOT : public ::testing::Test {
protected:
	ParallelSumTOT() {}
	virtual ~ParallelSumTOT() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

#define workload_size 10
TEST_F(ParallelSumTOT, default_grain_size) {
    WorkStealingScheduler scheduler;
    register_scheduler(scheduler);
    std::vector<int> v(workload_size);
    for(int i=0;i<workload_size;i++) v[i]=i;
    int sum=libasync_sum<1000>(v.begin(), v.end());
    EXPECT_EQ(sum, 45);
}