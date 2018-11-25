## std::reference_wrapper as a data member 

SpawnSyncTask is a subclass of Callable. Addition to Callable members, it has a reference to the scheduler. I find reference wrapper to be satisfyingly helpful in this scenario. 

#### SpawnSyncTask Case 

Why reference wrapper? What if we just use WorkStealingScheduler& as its data member? 
SpawnSyncTask's implicitly-declared copy/move assignment operators are deleted because SpawnSyncTask has a non-static data member of a reference type. So we have to define them manually like this.
```cpp
SpawnSyncTask& operator=(SpawnSyncTask&& other)noexcept{
    scheduler=std::ref(other.scheduler);
    Callable<R>::operator=(std::move(other));
    return *this;
}
SpawnSyncTask& operator=(const SpawnSyncTask& other)noexcept{
    scheduler=std::ref(other.scheduler);
    Callable<R>::operator=(other);
    return *this;
}	
```
Unfortunately hand crafting copy/move assignment operators will make implicitly-declared constructors deleted (which would not be deleted by default). So we have to define them altogether like this.
```cpp
SpawnSyncTask(SpawnSyncTask&& other):Callable<R>(std::move(other)), scheduler(scheduler){}
SpawnSyncTask(const SpawnSyncTask& other ):Callable<R>(other), scheduler(scheduler){}	
```

To get rid of these boring definitions, all we need to do is use std::reference_wrapper<WorkStealingScheduler> instead of  WorkStealingScheduler& as SpawnSyncTask's data member.