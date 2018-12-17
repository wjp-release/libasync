#pragma once
#include <string>
// Avoid mutithreaded print interleaving
// Auto end-line 
// Different colors for different threads 
namespace wjp{

#define SAMPLE_DEBUG

void println(const std::string& what);

}