# C++17

## 环境与工具
Windows下用VS2017+CMake，Linux或Mac下用VS Code+CMake+gdb，此外还可以用python脚本减少重复操作。

CMakeList.txt中win+linux双系统支持，sample/test双编译选项的项目结构可参照这个项目。

持续集成和代码覆盖所用的配置脚本亦然。

## C++语言

### structured binding 和 bit field
```cpp
int a[2] = {1,2};
 
auto [x,y] = a; // creates e[2], copies a into e, then x refers to e[0], y refers to e[1]
auto& [xr, yr] = a; // xr refers to a[0], yr refers to a[1]
```

```cpp
#include <set>
#include <string>
#include <iomanip>
#include <iostream>
 
int main() {
    std::set<std::string> myset;
    if (auto [iter, success] = myset.insert("Hello"); success) //if中也可定义变量了 
        std::cout << "insert is successful. The value is " << std::quoted(*iter) << '\n';
    else
        std::cout << "The value " << std::quoted(*iter) << " already exists in the set\n";
}

```

```cpp
struct S {
    int x1 : 2; //可用冒号数字指明所占bit数
    volatile double y1;
};
S f();
 
const auto [x, y] = f(); // x is a const int lvalue identifying the 2-bit bit field
                         // y is a const volatile double lvalue
```

```cpp
struct S {
 // three-bit unsigned field,
 // allowed values are 0...7
 unsigned int b : 3;
};
int main()
{
    S s = {7};
    ++s.b; // unsigned overflow (guaranteed wrap-around)
    std::cout << s.b << '\n'; // output: 0
}
```

### 标准库




### 类相关

override和final关键字，在virtual函数后表明这个函数一定是重写的，final表示最后的重写。

A& operator=(A other) 是联合赋值操作符，把T& operator=(T&& other) 和 T& operator=(const T& other)的效果合二为一了，正常情况下就用它。
不过如果other里面有真的要复制一份的资源，比如动态分配内存的array，那还是得用T& operator=(const T& other)手动进行复制。

```cpp
struct A
{
    int n;
    std::string s1;
    // user-defined copy assignment, copy-and-swap form
    A& operator=(A other)
    {
        std::cout << "copy assignment of A\n";
        std::swap(n, other.n);
        std::swap(s1, other.s1);
        return *this;
    }
};
 
struct B : A
{
    std::string s2;
    // implicitly-defined copy assignment
};
 
struct C
{
    std::unique_ptr<int[]> data;
    std::size_t size;
    // non-copy-and-swap assignment
    C& operator=(const C& other)
    {
        // check for self-assignment
        if(&other == this)
            return *this;
        // reuse storage when possible
        if(size != other.size)
        {
            data.reset(new int[other.size]);
            size = other.size;
        }
        std::copy(&other.data[0], &other.data[0] + size, &data[0]);
        return *this;
    }
    // note: copy-and-swap would always cause a reallocation
};
 
int main()
{
    A a1, a2;
    std::cout << "a1 = a2 calls ";
    a1 = a2; // user-defined copy assignment
 
    B b1, b2;
    b2.s1 = "foo";
    b2.s2 = "bar";
    std::cout << "b1 = b2 calls ";
    b1 = b2; // implicitly-defined copy assignment
    std::cout << "b1.s1 = " << b1.s1 << " b1.s2 = " << b1.s2 << '\n';
}
```
copy elision，更放心地用值语义，编译器会避免复制！返回值没有NRVO也无妨，反正有move语义也不会复制。copy elision发生的时候，copy/move ctor里的代码完全不会被执行。

```cpp
#include <iostream>
#include <vector>
 
struct Noisy {
    Noisy() { std::cout << "constructed\n"; }
    Noisy(const Noisy&) { std::cout << "copy-constructed\n"; }
    Noisy(Noisy&&) { std::cout << "move-constructed\n"; }
    ~Noisy() { std::cout << "destructed\n"; }
};
 
std::vector<Noisy> f() {
    std::vector<Noisy> v = std::vector<Noisy>(3); // copy elision when initializing v
                                                  // from a temporary (until C++17)
                                                  // from a prvalue (since C++17)
    return v; // NRVO from v to the result object (not guaranteed, even in C++17)
}             // if optimization is disabled, the move constructor is called
 
void g(std::vector<Noisy> arg) {
    std::cout << "arg.size() = " << arg.size() << '\n';
}
 
int main() {
    std::vector<Noisy> v = f(); // copy elision in initialization of v
                                // from the temporary returned by f() (until C++17)
                                // from the prvalue f() (since C++17)
    g(f());                     // copy elision in initialization of the parameter of g()
                                // from the temporary returned by f() (until C++17)
                                // from the prvalue f() (since C++17)
}
```





