## alloc优化
并行算法任务数量指数增长，会频繁进行内存分配。
如果基于programmer-friendly版本进行改进，可以采取下列措施：
1. arena，基于内存池的更快的allocator
2. scalable malloc，多线程alloc内存不必经过global lock。
3. 预先构造好的空白任务池，以及一个freelist记录可用任务。

cache-friendly版本进行更激进的优化，直接把所有task（固定大小，两个cacheline大）放在一起，组成wjp deque和wjp buffer。deque与buffer再组成worker，N个物理线程则对应N个worker。deque和buffer初始虽空，却实际上已经将空白任务初始化，以后新建任务只需移动base，top边界，并为新任务进行一次赋值。

从而省去了三个开销：任务push进容器的开销、分配内存的开销（尤其是多线程环境下还避免了默认malloc/new的全局锁）、task共通部分构造函数的开销。

## wjp deque/buffer

在Chase Lev Deque基础上加了3条规则：
1. take()不再左移top指针，而只是把自右向左搜索到的第一个非executing任务状态改成executing。当这个任务执行完施放之后，状态改成done，并加入freelist。（这里存在优化的空间，如何避免自右向左搜索开销？）
2. freelist是一个较小的单链表（taskheader里有一个task*next，就借助它进行链接），任务结束后加入freelist。
3. 若freelist不空，则push()不再右移top指针，而是直接利用freelist里的任务。这样可以避免freelist太大、top-base太大。


## 结构布局优化

在cache-friendly版本中，用户仍可以继承task，自定义各种各样的任务，但又保证这些任务都被装在taskblock里。taskblock是deque，buffer分配内存的最小单位，内含taskheader和各种各样task本体，总是补齐到128字节，即两个cache line size。

这样有两个好处：
1. 防止false sharing，如果task太小，两个任务位于同一个cache line里，B任务修改会导致A任务cache line不必要地陷入dirty状态。
2. 提高cache hit rate。强制128字节，强制从deque和buffer数组中初始化取用task，可保证所有任务cache-aligned，且刚好fit 1到2各cache line。

## Cancel&Exec算法的改良

所谓Cancel&Exec就是指programmer-friendly版本中父任务在wait时会尝试取消自己创建的最新子任务，如果没被偷走取消成功就可以直接在wait routine里把它就地执行完。

cache-friendly版本中直接规避了最新子任务被偷这个可能性，也不用先放进deque再cancel了，直接提供一个接口：emplaceLastTaskAndWait()，百分百保证最后一个子任务在父任务wait的时候立刻执行，不会被偷走。（实现时还是在deque里构造的，只不过构造之初就把状态设为executing了）

## wait routine和worker routine中的steal策略

frogleaping算法的变种（可优化）

## wait routine中的helping策略

programmer-friendly版本里的策略基本上足够好了：优先deque本地的任务，从新到旧（越新cache越热，利用temporal locality），找不到再偷其他worker的（偷显然cache-unfriendly）。

cache-friendly中额外增加一个更高优先级的任务——execute返回的任务，这个任务由用户自己指定，可以为nullptr。假如一个任务并不wait自己的子任务，那么返回最新子任务也可以让helping优先选择这个最新子任务。