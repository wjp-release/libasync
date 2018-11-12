## work stealing（without continuation）

work stealing是一种并行计算的调度算法，Java和.NET都采用的实现。

它采用fork-join模型，让秩序路径等价于一个有向无环图（即DAG）。

每个物理线程上有一个无锁并发任务deque，有push，pop和poll。其中push和pop由所属线程调用，poll由其他线程调用(即偷窃)。

task特殊操作如下：fork()：先new一个子任务，调用它的fork可将它push到deque里。join()：等待执行结束。

一个pool有n个worker线程，2n个workqueue，其中一半workqueue和worker线程绑定，另一半专门用来submit外来任务。
外来任务的submit由外来线程执行，执行完就使任务进入某个hash过后得到的queue中，随后就会迅速被worker thread偷走执行掉。

### 优化

优化1：为了避免worker无任务可偷时spin wait浪费资源，pool还会将这些空的worker加入闲置队列，timeout之后就销毁。

优化2：workqueue记录它上次偷取的目标workqueue的id，这个信息可以帮助join加速：如果一个workqueue的任务A被偷了，它想join A，就得帮偷了A的queue解决A分解出来的子任务，所以它找到偷了自己的workqueue再反偷可以加速这个A任务家族的终结。

优化3：自动根据负载调整worker thread的数量，由task submission，replacement of terminated worker来触发。

###  work stealing vs work sharing
work sharing是用一个公共阻塞task queue，分发任务到空闲的线程里。

work stealing让每个线程拥有自己的任务队列，优先执行自己spawn出来的新任务，没有任务时主动去偷窃别的线程的任务，有下列优势：
1. 控制上去中心化，全局单一任务队列的吞吐量有限，在海量小人物spawn的情况下尤其明显。
2. work sharing每次调度必然发生task data migration，stealing在处理spawned tasks时就是不需要migration的。
3. work sharing是一定会休眠才被认为是空闲，然后再被唤醒，stealing如果空了会主动去偷，经常省去了休眠唤醒的过程。



    
