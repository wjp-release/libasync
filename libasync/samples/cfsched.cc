#include "cfsched.h"
#include "Internal.h"
#include "CFTask.h"

using namespace wjp::cf;

struct AddOne : public Task{ 
public:
	AddOne(int v):value(v){}
protected:
	int value;
	Task* compute() override{
		if(value==30) return nullptr;
		//AddOne* child1=spawnDetached<AddOne>(value+1);
		//AddOne* child2=spawnDetached<AddOne>(value+1);
		if constexpr(PrintTestTaskOutput) wjp::println(std::to_string(value));		
		return nullptr;
	}
};

void taskexec(){
    TaskPool& pool=TaskPool::instance();
	AddOne* root_task=TaskPool::instance().emplaceExternally<AddOne>(10);
    //wjp::sleep(1000);
    root_task->execute();
}

void cfsched(){
    TaskPool& pool=TaskPool::instance();
    pool.start();
    Worker& worker=pool.getWorker(2);
    wjp::println(worker.stat());
    pool.terminate();
}