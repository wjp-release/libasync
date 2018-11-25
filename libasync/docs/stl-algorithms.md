## STL Algorithms

### 高级语言组件

STL Algorithm里值得注意的一点是它在尝试取代传统的for+if的控制循环，从而为c++提供一种（非常）高级语言的表达范式。

因此，虽然改变起来比较痛苦，我觉得能用fill, any_of就用，形成习惯后会更可读、可维护。


### 并行算法

C++17之后可以指定三种算法执行策略：线性、并行、无序并行。第三种实际上就意味着可被work-stealing算法调度。

```cpp
int x = 0;
std::mutex m;
int a[] = {1,2};
std::for_each(std::execution::par, std::begin(a), std::end(a), [&](int) {
  std::lock_guard<std::mutex> guard(m);
  ++x; // 用户代码要自己保证不能死锁
});
```


### 函数式编程

以count_if为例，algorithm里的很多算法需要用户提供函数做参数，每次都写lambda可读性也不好，所以用户自己可以定义一些functor，

很多实用的用户定义的functor，以及functor工厂可以在boost/hof中找到。



