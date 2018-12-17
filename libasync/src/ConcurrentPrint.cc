#include "ConcurrentPrint.h"
#include <thread>
#include <iostream>
#include "PrintColor.h"
#include "ThreadUtilities.h"

namespace wjp{

static std::mutex mtx;

void println(const std::string& what){
    std::lock_guard<std::mutex> lk(mtx);
    std::cout<< (Color::Code)(Color::fg_red+(current_thread_hashed_id()%7))
        << what<< Color::fg_default <<std::endl;
}


}