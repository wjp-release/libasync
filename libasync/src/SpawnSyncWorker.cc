#include "SpawnSyncWorker.h"
#include "SpawnSyncPool.h"
#include "SpawnSyncTask.h"

namespace wjp{
	SpawnSyncWorker::SpawnSyncWorker(SpawnSyncPool&pool) : pool(pool)
	{
	}

	SpawnSyncWorker::~SpawnSyncWorker()
	{
		shutdown();
	}

	// Creates the worker thread that runs worker_routine
	void SpawnSyncWorker::start()
	{
		worker_thread=std::thread([this]{
			worker_routine();
		});
	}

	// Blocks the worker thread untill it is waked by SpawnSyncPool
	void SpawnSyncWorker::stall()
	{

	}
	
	void SpawnSyncWorker::sync(std::shared_ptr<SpawnSyncTask> target_task, std::chrono::milliseconds timeout)
	{

	}

	void SpawnSyncWorker::cancel(std::shared_ptr<Task> task)
	{

	}

	void SpawnSyncWorker::shutdown()
	{
		is_shutdown=true;
		condition_stall.notify_all();
		worker_thread.join();
	}

	void SpawnSyncWorker::worker_routine()
	{
		while (!is_shutdown) {
			if (find_next_task()) {   // find work to do or stay idle less than idle_timeout
				became_idle=std::nullopt;
			} else { // cannot find a task to run
				if(became_idle.has_value()){ // already idle
					auto elapsed=ms_elapsed_count(became_idle.value());
					if(elapsed>idle_timeout){ // idle timeout, force worker thread to wait
						became_idle=std::nullopt;
						std::unique_lock<std::mutex> lk(mtx);
						condition_stall.wait(lk,[this]{return !is_stalling;});
					}
				}else{ // used to be busy, becomes idle now
					became_idle.emplace<time_point>(now());
				}
			}
		}
	}

	bool SpawnSyncWorker::find_next_task()
	{

		return true;
	}

	void SpawnSyncWorker::execute_stolen_tasks()
	{
		
	}


	// Pushes a task into local task queue.
	void SpawnSyncWorker::push(std::shared_ptr<SpawnSyncTask>)
	{

	}

	// Takes the latest pushed task from local task queue.
	std::shared_ptr<SpawnSyncTask> SpawnSyncWorker::take()
	{

	}

	// Offers a least recently pushed task to stealers.
	std::shared_ptr<SpawnSyncTask> SpawnSyncWorker::steal()
	{

	}

	// Pushes a task into submission task queue. Should be called by a non-worker thread.
	void SpawnSyncWorker::submit(std::shared_ptr<SpawnSyncTask>)
	{

	}

	// Takes the latest pushed task from submission task queue. Should be called by a non-worker thread.
	std::shared_ptr<SpawnSyncTask> SpawnSyncWorker::undo_submit()
	{

	}


}