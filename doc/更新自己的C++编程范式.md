# C++17

## �����빤��
Windows����VS2017+CMake��Linux��Mac����VS Code+CMake+gdb�����⻹������python�ű������ظ�������

CMakeList.txt��win+linux˫ϵͳ֧�֣�sample/test˫����ѡ�����Ŀ�ṹ�ɲ��������Ŀ��

�������ɺʹ��븲�����õ����ýű���Ȼ��

## C++����

### structured binding �� bit field
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
    if (auto [iter, success] = myset.insert("Hello"); success) //if��Ҳ�ɶ�������� 
        std::cout << "insert is successful. The value is " << std::quoted(*iter) << '\n';
    else
        std::cout << "The value " << std::quoted(*iter) << " already exists in the set\n";
}

```

```cpp
struct S {
    int x1 : 2; //����ð������ָ����ռbit��
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

### ��׼��




### �����

override��final�ؼ��֣���virtual����������������һ������д�ģ�final��ʾ������д��

A& operator=(A other) �����ϸ�ֵ����������T& operator=(T&& other) �� T& operator=(const T& other)��Ч���϶�Ϊһ�ˣ���������¾�������
�������other���������Ҫ����һ�ݵ���Դ�����綯̬�����ڴ��array���ǻ��ǵ���T& operator=(const T& other)�ֶ����и��ơ�

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
copy elision�������ĵ���ֵ���壬����������⸴�ƣ�����ֵû��NRVOҲ�޷���������move����Ҳ���Ḵ�ơ�copy elision������ʱ��copy/move ctor��Ĵ�����ȫ���ᱻִ�С�

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





