#pragma once

// ----- Task canceling has very little effect on performance -----
#define WITH_CANCEL

// --------- Custom Worker Deque -----------
/* An optimised version of lock-free chase-lev deque */
#define WorkerDeque ChaseLevDeque   

/* A very simple task deque based on std::mutex */
//#define WorkerDeque SimpleSpawnDeque  


// ----- Count number of tasks -----
//#define INTERNAL_STATS 


//#define SAMPLE_DEBUG
