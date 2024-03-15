---
title: c++中的static和const 
date: 2023/12/16
---

```
cpp中类的初始化顺序：分配空间->依次初始化成员变量(如有，按成员顺序使用初始化列表初始化)->执行构造函数
cpp中类的析构顺序：析构函数->声明的反顺序执行成员变量析构函数->上层析构函数
```

# const

## const 变量

文件域的const变量默认是文件内可见的，如果需要在b.cpp中使用a.cpp中的const变量M，需要在M的初始化处增加extern，所有extern const语句中只能由一条声明语句。

```cpp
//a.cpp
extern const int M = 20;

//b.cpp
extern const int M;
```

## const 成员变量

类中的非static常量必须在构造函数的初始化列表中进行初始化，因为类中的非static成员是在进入构造函数的函数体之前就要构造完成的，而const常量在构造时就必须初始化，构造后的赋值会被编译器阻止。

```cpp
class B {
public:
  B(): name("aaa") {
    name = "bbb"; // !error
  }
private:
  const std::string name = "bbb"; //正确，但被"aaa"覆盖
};
```

## const 成员函数

C++中可以用const去修饰一个类的非static成员函数，其语义是保证该函数所对应的对象本身的const性。在const成员函数中，所有可能违背this指针const性（const成员函数中的this指针是一个双const指针）的操作都会被阻止，如对其它成员变量的赋值以及调用它们的非const方法、调用对象本身的非const方法。但对一个声明为mutable的成员变量所做的任何操作都不会被阻止。

# static

## static 成员变量

静态成员在类的所有对象中是共享的。如果不存在其他的初始化语句，在创建第一个对象时，所有的静态数据都会被初始化为零。我们不能把静态成员的初始化放置在类的定义中，但是可以在类的外部通过使用范围解析运算符 **::** 来重新声明静态变量从而对它进行初始化，如下面的实例所示。

```cpp
#include <iostream>

using namespace std;

class C {
  public:
    static int i;
};

int C::i = 1;

int main()
{
  C c;
  cout << C::i << endl;
}
```

## static 成员函数

- 静态成员函数没有 this 指针，只能访问静态成员（包括静态成员变量和静态成员函数）。
- 普通成员函数有 this 指针，可以访问类中的任意成员；而静态成员函数没有 this 指针。

# const static

##  const static 成员变量

static常量是在类中直接声明的，但要在类外进行唯一的定义和初始值，常用的方法是在对应的.cpp中包含类的static常量的定义：

```cpp
// a.h
class A {
    ...
    static const std::string name;
};
 
// a.cpp
const std::string A::name("aaa");
```

一个特例是，如果static常量的类型是内置的整数类型，如char、int、size_t等，那么可以在类中直接给出初始值，且不需要在类外再进行定义了。编译器会将这种static常量直接替换为相应的初始值，相当于宏替换。但如果在代码中我们像正常变量那样使用这个static常量，如取它的地址，而不是像宏一样只使用它的值，那么我们还是需要在类外给它提供一个定义，但不需要初始值了（因为在声明处已经有了）。

```cpp
// a.h
class A {
    ...
    static const int SIZE = 50; 
};
 
// a.cpp
const int A::SIZE = 50; // if use SIZE as a variable, not a macro
```

## const static 成员函数

不可以同时用const和static修饰成员函数。
C++编译器在实现const的成员函数的时候为了确保该函数不能修改类的实例的状态，会在函数中添加一个隐式的参数const this*。但当一个成员为static的时候，该函数是没有this指针的。也就是说此时const的用法和static是冲突的。

我们也可以这样理解：两者的语意是矛盾的。static的作用是表示该函数只作用在类型的静态变量上，与类的实例没有关系；而const的作用是确保函数不能修改类的实例的状态，与类型的静态变量没有关系。因此不能同时用它们。

---

**参考：《C++ Primer》、[C++总结：C++中的const和constexpr](https://www.cnblogs.com/fuzhe1989/p/3554345.html)、[C++对象构造与析构全流程_对象的构造和析构的具体过程是什么](https://blog.csdn.net/Innocent_code/article/details/87985080)、[C++ 类的静态成员](https://www.runoob.com/cplusplus/cpp-static-members.html)、[static 和const分别怎么用，类里面static和const可以同时修饰成员函数吗?_const放在函数中的用法](https://blog.csdn.net/leigelaile1/article/details/80430162)。**
