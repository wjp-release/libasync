#pragma once

#include "Async.h"
#include <numeric>

// Used in benchmarks against tbb::parallel_reduce and std::async.

namespace wjp{

template <int GrainSize, typename RandomIt>
int libasync_sum(RandomIt beg, RandomIt end)
{
    auto len = end - beg;
    // Note that std::accumulate is actually slower than a for-loop adding-up ...
    if (len < GrainSize) return std::accumulate(beg, end, 0); 
    RandomIt mid = beg + len/2;
    auto subtask=libasync_sum<GrainSize,RandomIt>|async<int>(mid,end);
    return libasync_sum<GrainSize>(beg, mid) + subtask->join();
}


}