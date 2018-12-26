#pragma once
#include <string>
namespace wjp{

//#define SAMPLE_DEBUG

// Avoid mutithreaded print interleaving
// Auto end-line 
// Different colors for different threads 
void println(const std::string& what);

}