## 元编程

### 局部特化
```cpp
template<class T1, class T2, int I>
class A {};            // primary template
 
template<class T, int I>
class A<T, T*, I> {};  // #1: partial specialization where T2 is a pointer to T1
 
template<class T, class T2, int I>
class A<T*, T2, I> {}; // #2: partial specialization where T1 is a pointer
 
template<class T>
class A<int, T*, 5> {}; // #3: partial specialization where T1 is int, I is 5,
                        //     and T2 is a pointer
 
template<class X, class T, int I>
class A<X, T*, I> {};   // #4: partial specialization where T2 is a pointer
```

###