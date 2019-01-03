#pragma once

#include <exception>
#include <stdexcept>

namespace wjp::cf{

// This exception is thrown by <emplace> operation if freelist is empty.
// Note that CFDeque is essentially a per-worker memory pool of tasks; it's very large.
// You should handle this exception in the same way you handle a std::bad_alloc. 

class TooManyTasks : public std::runtime_error{
public:
    TooManyTasks() : std::runtime_error("You have created more tasks than TaskNumberPerWorker which is configurable in CFConfig.h.") {}
};


}