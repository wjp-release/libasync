#include "SpawnSyncPool.h"
#include "SpawnSyncWorker.h"
#include "SpawnSyncTask.h"

namespace wjp{
	SpawnSyncPool::SpawnSyncPool(){
		auto nr_cpus = std::thread::hardware_concurrency();
		workers.reserve(nr_cpus*2); 
	}
	bool SpawnSyncPool::is_shutdown(){
		return state==SHUTDOWN;
	}
	bool SpawnSyncPool::is_terminiated(){
		return state==TERMINATED;
	}
	int SpawnSyncPool::get_state(){
		return state;
	}

	void SpawnSyncPool::shutdown()
	{

	}

	std::list<std::shared_ptr<Runnable>> SpawnSyncPool::stop()
	{

	}

	void SpawnSyncPool::on_stopped()
	{

	}

	bool SpawnSyncPool::wait_till_terminated(std::chrono::milliseconds timeout)
	{

	}

	std::shared_ptr<SpawnSyncTask> SpawnSyncPool::run(std::shared_ptr<Runnable>)
	{
		
	}

	void SpawnSyncPool::push_from_outsider(std::shared_ptr<SpawnSyncTask>)
	{

	}

	void SpawnSyncPool::wait_till_every_tasks_has_terminated()
	{

	}

	bool SpawnSyncPool::every_tasks_has_terminated()
	{

	}

	void SpawnSyncPool::activate_idle_worker_if_necessary()
	{
		
	}

}