#include "cfsched.h"
#include "Internal.h"

using namespace wjp::cf;
using namespace wjp;

void cfsched(){
    TaskPool& pool=TaskPool::instance();
    pool.start();
    Worker& worker=pool.getWorker(2);
    println(worker.stat());
    pool.terminate();
}