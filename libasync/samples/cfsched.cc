#include "cfsched.h"
#include "Internal.h"
#include "CFTask.h"
#include <iostream>
#include "CFParallelSum.h"
#include "RandomUtilities.h"

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
	AddOne* root_task=TaskPool::instance().emplaceRoot<AddOne>(10);
	std::cin.get();
}

void cfsched(){
    TaskPool& pool=TaskPool::instance();
    pool.start();
    Worker& worker=pool.getWorker(2);
    wjp::println(worker.stat());
    pool.terminate();
}

#define workload_size 100

void parallelsum(){
	TaskPool::instance().start();
    std::vector<int> v(workload_size);
	int c=0;
    for(int i=0;i<workload_size;i++){
        v[i]=wjp::randint<0,10>();
		c+=v[i];
    }		
	int sum=parallelSum<10>(v.data(),v.size());
	std::cout<<"sum="<<sum<<", c="<<c<<std::endl;
}

