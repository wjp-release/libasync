#include "cfsched.h"
#include "Internal.h"
#include "CFTask.h"
#include <iostream>

using namespace wjp::cf;

struct AddOne : public Task{ 
public:
	AddOne(int v):value(v){}
protected:
	int value;
	std::string stats() override{
		return Task::stats()+"("+std::to_string(value)+")";
	}
	Task* compute() override{
		wjp::sleep(500); 
		if(value==17) return nullptr;
		AddOne* child1=spawnDetached<AddOne>(value+1);
		AddOne* child2=spawnDetached<AddOne>(value+1);
		if constexpr(PrintTestTaskOutput) wjp::println(std::to_string(value));		
		return nullptr;
	}
};

void taskexec(){
	TaskPool::instance().start();
	AddOne* root_task=TaskPool::instance().emplaceExternally<AddOne>(10);
	std::cin.get();
}

void cfsched(){
    TaskPool& pool=TaskPool::instance();
    pool.start();
    Worker& worker=pool.getWorker(2);
    wjp::println(worker.stat());
    pool.terminate();
}

