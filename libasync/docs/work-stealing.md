## work stealing

work stealing是一种并行计算的调度算法。

它采用fork-join模型，让一个可分解任务执行路径等价于一个有向无环图（即DAG）。

每个物理线程上有一个无锁并发任务deque，有push，pop和poll。其中push和pop由所属线程调用，poll由其他线程调用(即偷窃)。

task特殊操作如下：fork()：先new一个子任务，调用它的fork可将它push到deque里。join()：等待执行结束。

一个pool有n个worker线程，2n个workqueue，其中一半workqueue和worker线程绑定，另一半专门用来submit外来任务。
外来任务的submit由外来线程执行，执行完就使任务进入某个hash过后得到的queue中，随后就会迅速被worker thread偷走执行掉。

### juc的优化

优化1：为了避免worker无任务可偷时spin wait浪费资源，pool还会将这些空的worker加入闲置队列，timeout之后就销毁。

优化2：workqueue记录它上次偷取的目标workqueue的id，这个信息可以帮助join加速：如果一个workqueue的任务A被偷了，它想join A，就得帮偷了A的queue解决A分解出来的子任务，所以它找到偷了自己的workqueue再反偷可以加速这个A任务家族的终结。

优化3：在某些worker stall时，启用备用worker补偿这种parallelism的损失。

###  work stealing vs work sharing
work sharing是用一个公共阻塞task queue，分发任务到空闲的线程里。

work stealing让每个线程拥有自己的任务队列，优先执行自己spawn出来的新任务，没有任务时主动去偷窃别的线程的任务，有下列优势：
1. 控制上去中心化，全局单一任务队列的吞吐量有限，在海量小人物spawn的情况下尤其明显。
2. work sharing每次调度必然发生task data migration，stealing在处理spawned tasks时就是不需要migration的。
3. work sharing是一定会休眠才被认为是空闲，然后再被唤醒，stealing如果空了会主动去偷，经常省去了休眠唤醒的过程。

### strict work stealing
只允许等待自己的child task，让fork/join就像是函数调用一样，可以封装在特殊并行函数里，这种严格work stealing可以防止task leaking。任务泄露比内存泄露更严重，非常致命。严格化fork join机制通过降低灵活性提高了编程的简易程度。

### pdf vs work stealing
pdf是另一个soa的线程池任务调度算法，通过parallel depth first，把优先级（priority等于parent nodes中经历step数目的最大值）最高的任务优先安排执行，使得并行程序的执行顺序更类似线性程序，从而获得更好的L2 cache（L2或者L3，共享的大cache）命中。为何线性程序cache性能就好？因为编译器还有各种语言的固定范式就是在一个小工作集上进行模块化的操作，天然就很容易产生locality。相隔较远的两个操作很容易使用截然不同的工作集。


### parent stealing vs child stealing
如果只有一个线程，那么parent stealing的行为和线性程序一致。parent stealing最好要有语言级别的continuation支持，让任务在fork时就立刻执行child任务，把自己剩余的代码（continuation）enqueue，然后给别的worker偷。

child stealing比较好实现，就是让新建的task进queue，然后继续执行原本的父任务的代码。

### stalling vs non-stalling
stalling是让发起join的父任务所在worker阻塞，等待子任务结束的方法。（可以采用补偿法，在这期间唤醒一个备用worker线程，避免阻塞带来的并行能力损失）

non-stalling则是让最后一个抵达join（或sync标记）的任务（有可能是子任务）负责执行join之后的父任务代码。这也需要continuation的支持。

在缺乏continuation支持的情况下，用户级的库采用stalling+child stealing会容易实现一点。




    
