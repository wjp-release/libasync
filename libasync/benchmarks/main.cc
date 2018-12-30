#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "parallel_sum_benchmark.h"

int main() {
    std::cout<<"parallel_sum_benchmark_grainsize_1000\n";
    parallel_sum_benchmark_grainsize_1000();
    std::cout<<"parallel_sum_benchmark_grainsize_5000\n";
    parallel_sum_benchmark_grainsize_5000();
    return 0;
}

