# libasync

[![Build Status](https://travis-ci.org/wjp-release/libasync.svg?branch=master)](https://travis-ci.org/wjp-release/libasync)
[![Build status](https://ci.appveyor.com/api/projects/status/wy1bgkp4lo5ryt8n/branch/master?svg=true)](https://ci.appveyor.com/project/wjpjw/libasync/branch/master)

provides high level building blocks for asynchronous computation in latest C++

## Why libasync?
#### Fast
parallel_sum using libasync is 5 times faster than the C++11 std::async version. (see https://en.cppreference.com/w/cpp/thread/async)

#### Correct
Even though lock-free algorithms are in general premature, the correctness of the one used in libasync is formally proved.

#### Simplistic API
Pipable API 

## How to use libasync

1) Copy libasync/libasync into your cmake project directly,
2) then use `add_subdirectory(libasync)` to build it as a part of your project,
3) use `include_directories(./libasync/libasync/src/)` and `link_directories(./libasync/libasync/src/)` to set include and link paths.

## 
