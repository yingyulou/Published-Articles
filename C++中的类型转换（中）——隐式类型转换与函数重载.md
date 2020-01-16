# C++中的类型转换（中）——隐式类型转换与函数重载

在上篇中，我们讨论了C++中与隐式类型转换相关的一些话题，而函数重载是与隐式类型转换相关的又一大重要话题，本篇将要讨论的内容即为隐式类型转换与函数重载之间的相关话题。

## 6. 隐式类型转换与重载确定

C++中，如果同时定义了多个函数名称相同，但函数签名不同的函数，则此行为称为函数重载。调用重载函数时，编译器将根据调用的参数数量与类型确定被调用的是哪一个函数，此过程称为重载确定。在重载确定过程中，如果编译器发现不止一个函数都是当前调用的最佳版本，则将引发二义性编译时错误。

需要声明的是：**重载确定是一个非常复杂的话题，本文没有对重载确定的所有情况进行详尽的论述，而只是提出了其中一些与隐式类型转换关联较大的，或具有代表性的话题进行论述。**

首先，引用《C++ Primer》中对于重载确定的隐式类型转换等级的说明：

> 为了确定最佳匹配，编译器将实参类型到形参类型的转换划分成几个等级，具体排序如下所示：
>
> 1. 精确匹配，包括以下情况：
>
> - 实参类型和形参类型相同
>
> - 实参从数组类型或函数类型转换成对应的指针类型
>
> - 向实参添加顶层const或从实参中删除顶层const
>
> 2. 通过const转换实现的匹配
>
> 3. 通过类型提升实现的匹配
>
> 4. 通过算术类型转换或指针转换实现的匹配
>
> 5. 通过类类型转换实现的匹配

根据这段内容，我们可以得到以下要点：

1. 数组向指针，以及函数向函数指针的隐式类型转换并不被C++视为隐式类型转换，其只是语法上的差别
2. 显然，顶层const在不同变量之间不存在传递性
3. 涉及底层const的类型转换被视为最接近精确匹配的隐式类型转换
4. 类型提升优先于算术类型转换
5. 自定义的类型转换等级最低，**且各种自定义类型转换之间无先后差别**

我们首先来看一个最简单的例子：

``` Cpp
void test(int) {}
void test(int, int = 0) {}

int main()
{
    test(0);  // 调用哪一个test？
}
```

上述代码中，我们仅通过一个int调用了test函数，此时，编译器无法确定调用的函数版本应该是void test(int)还是void test(int, int = 0)，所以此调用是具有二义性的。

再来看一个根据形参类型进行重载确定的例子：

``` Cpp
void test(int) {}
void test(double) {}

int main()
{
    test(0);    // void test(int)
    test(0.);   // void test(double)
    test('0');  // void test(int)，因为char -> int比char -> double更优
}
```

上述代码中，显然，test(0)调用是void test(int)版本的精确匹配，且test(0.)调用是void test(double)版本的精确匹配。考察test('0')，由于char -> int属于类型提升，其比char -> double更优，故编译器选择了void test(int)版本，并将char通过隐式类型转换转为int。

接下来讨论用户自定义的类型转换与重载确定之间的关系。

首先，C++对于隐式类型转换有一条非常重要的规则：**对于单次隐式类型转换，用户定义的隐式类型转换方案与算术类型转换各可出现一次，且顺序不限。**

围绕此条性质，首先可以得到的结论是：不允许出现不止一次的用户定义的隐式类型转换。

参考以下代码：

``` Cpp
// 定义A -> B -> C的隐式类型转换
struct A {};
struct B { B(const A &) {} };
struct C { C(const B &) {} };

int main()
{
    B b  = A();     // A -> B
    C c1 = A();     // Error! 不可以连续使用两次自定义的隐式类型转换
    C c2 = B(A());  // B -> C
}
```

上述代码中，使用A类对象对B类变量赋值是可行的，因为我们定义了A -> B的转换构造函数，但如果我们试图通过多个转换构造函数实现A -> B，然后B -> C的隐式类型转换，则会引发编译时错误。

对于同时存在算术类型转换与用户定义的类型转换的情况，重载确定将分为以下几种情况：

1. 存在精确匹配的类型转换

参考以下代码：

``` Cpp
struct A { operator int() { return 0; } operator double() { return 0.; } };

int main()
{
    static_cast<double>(A());
}
```

上述代码中，A类可以同时向int与double进行类型转换，由于A -> double是operator double的精确匹配，则编译器将选择此版本。

2. 存在多个可行的用户定义的类型转换，但不存在精确匹配的类型转换

参考以下代码：

``` Cpp
struct A { operator int() { return 0; } operator double() { return 0.; } };

int main()
{
    static_cast<long double>(A());  // 二义性错误！
}
```

上述代码中，类型转换的目标由1中的A -> double修改为A -> long double，由于A类定义的两个类型转换都不再是精确匹配，此时编译器将直接判定此行为具有二义性。

再来看下一个例子：

``` Cpp
// 同时定义int -> A, int -> B, double -> C的转换构造函数
struct A { A(int) {} };
struct B { B(int) {} };
struct C { C(double) {} };

void test(const A &) {}
void test(const B &) {}
void test(const C &) {}

int main()
{
    // 由于同时存在多个可行的用户定义的类型转换，包括：
    // 1. 0 -> A
    // 2. 0 -> B
    // 3. 0 -> double -> C
    // 此调用将直接被判定为二义性
    test(0);

    // 同理，这样的调用存在的可行的类型转换如下：
    // 1. 0. -> int -> A
    // 2. 0. -> int -> B
    // 3. 0. -> C
    // 故即使0. -> C是精确匹配，也将被判定为二义性
    test(0.);
}
```

上述代码中，我们为类A，B，C都定义了int或double到类类型的转换构造函数，同时，我们定义了分别以A，B，C作为形参类型的三个重载函数，并使用int或double作为实参进行调用。此时，由于int与double都存在向A，B，C进行基于用户定义的隐式类型转换的方案，故此种调用将直接被编译器判定为二义性。

由此，我们得到了第一条用户定义的类型转换方案与重载确定之间的重要规则：**如果存在多个可行的用户定义的类型转换，且没有一个类型转换是精确匹配的，则此种情况将被编译器判定为二义性。**

3. 只存在一个可行的用户定义的类型转换，但不是精确匹配

参考以下代码：

``` Cpp
struct A { operator int() { return 0; } };

int main()
{
    static_cast<long double>(A());  // A -> int -> long double
}
```

上述代码中，根据上文“用户定义的隐式类型转换方案与算术类型转换各可出现一次，且顺序不限”的性质，此种情况是可行的。当我们试图将A转换为long double时，A()通过其唯一的类型转换操作符operator int()被转换为一个int，然后再通过算术类型转换转为long double。

4. 只存在一个用户定义的类型转换，但存在多个算术类型转换

参考以下代码：

``` Cpp
struct A { A(int) {} A(double) {} };

int main()
{
    A('0');  // char -> int优于char -> double，故进行char -> int -> A的类型转换
    A(0l);   // long -> int与long -> double都不是更优的算术类型转换，故此调用具有二义性
}
```

首先需要明确的是，“不同的类型转换”指多个转换目标不同的类型转换，与转换起点无关，由于上述代码中的两个转换构造函数的转换目标都是A，所以其并不是两个不同的类型转换，即不是上述第二点所述的情况。

对于此例，我们首先需要引入第二条重要规则：**如果只存在一个可行的用户定义的类型转换，但存在多个不同的算术类型转换时，重载确定以算术类型转换的优劣作为依据。**

考察对A的两次实例化：A('0')涉及两种隐式类型转换：char -> int -> A与char -> double -> A，由于char -> int的算术类型转换等级较之char -> double更高，故编译器选择了char -> int -> A的隐式类型转换方案。而对于A(0l)，由于long -> int与long -> double都不是更好的算术类型转换，故编译器判定此调用为二义性。

## 7. 隐式类型转换与函数模板

### 7.1 隐式模板实例化与隐式类型转换

C++中，函数模板通过模板实参推导过程实例化出最适合于当前调用参数的函数版本，那么显然，所有实例化出的函数版本互为重载函数，即：这些实例化函数之间不能存在任何的二义性。同时，由于函数模板对于实参类型的高度不确定性，隐式类型转换几乎不会发生在模板实例化过程中，取而代之的是一个新的函数版本。为了同时满足“最适合当前调用参数的版本”，以及“互为重载函数”这两个要求，我们不难发现，模板实例化时必须要忽略少量类型转换，而这正是重载确定中被视为“精确匹配”的条目，包括以下几点：

1. 顶层const的有无
2. 数组到指针的类型转换

参考以下代码：

``` Cpp
template <typename T>
void test(T) {}

int main()
{
    const char testStr[10] = "";
    const char * const strPtr = "";
    test(testStr);  // T = const char *，const char [10]转为const char *
    test("");       // T = const char *
    test(strPtr);   // T = const char *，const char * const转为const char *
}
```

上述代码中，虽然我们使用const char [10]类型的变量调用模板函数，编译器依然会实例化出一个test(const char *)版本进行调用，这是由“数组即指针”这一底层特性决定的。而对于const char * const类型，显然，编译器将忽略其顶层const。

### 7.2 显式模板类型，显式模板实例化与隐式类型转换

对于模板中的任何显式类型部分，其都遵循隐式类型转换规则。这主要分为两种情况：

1. 显式类型形参

参考以下代码：

``` Cpp
template <int = 0>
void test(int) {}

int main()
{
    test(0.);  // double -> int
}
```

上述代码中，虽然test是模板函数，但其第一参数是一个明确的int类型，故传入的double类型实参将通过隐式类型转换被转换为int类型。

2. 显式模板实例化

参考以下代码：

``` Cpp
template <typename T>
void test(T) {}

int main()
{
    test<int>(0.);  // 强制调用void test(int)版本，double -> int
}
```

上述代码中，我们通过显式模板实例化，强行构造并调用了一个void test(int)版本的函数。则此时，实参的double类型将通过隐式类型转换被转换为int类型。

### 7.3 引用折叠

引用折叠是另一种较为复杂的类型转换。参考以下代码：

``` Cpp
template <typename T>
void test(T &) {}

template <typename T>
void test(T &&) {}

int main()
{
    const int &a = 0;
    const int &&b = 0;
    test(a);             // 调用void test(int & &)   -> 折叠为void test(int &)
    test(b);             // 调用void test(int && &)  -> 折叠为void test(int &)
    test(std::move(a));  // 调用void test(int & &&)  -> 折叠为void test(int &)
    test(std::move(b));  // 调用void test(int && &&) -> 折叠为void test(int &&)
}
```

当模板参数声明为一个引用，且调用参数也为一个引用时，模板实例化出的参数类型将出现“引用的引用”，这包括以下四种情况：

1. 使用T &调用T &：T & &
2. 使用T &&调用T &：T && &
3. 使用T &调用T &&：T & &&
4. 使用T &&调用T &&：T && &&

当出现以上情况时，通过模板实例化出的函数将发生引用折叠。情况1，2，3将折叠为T &，情况4将折叠为T &&。

需要注意的是，引用折叠只是对实参的适配手段，并不改变T的类型。即：如果使用T &调用T &&，则T的类型就是T &。

## 8. 函数模板的重载确定

对于模板重载，首先需要明确以下几个要点：

1. 模板也可以重载，各模板函数之间的函数签名应互不相同
2. 模板函数与非模板函数可并存，共同作为同一重载体系
3. 模板特化隶属于某个模板函数的强制实例化版本，与函数重载无关（重载确定后，如果确实调用了具有模板特化的模板函数，此时才会考虑模板特化）

如果重载函数中具有模板函数，则此时重载确定同样遵循普通函数的重载确定规则，以及以下的几点新规则：

1. 精确匹配的非模板函数的优先级大于有能力通过实例化得到精确匹配的模板函数
2. 普适性更低的模板函数的优先级大于普适性更高的模板函数

首先我们讨论上述第一点规则，参考以下代码：

``` Cpp
template <typename T>
void test(T) {}

template <>
void test(double) {}

template <typename T>
void test(T, T) {}

template <>
void test(int, int) {}

void test(double) {}
void test(int, double) {}

int main()
{
    test(0.);     // 调用非模板函数void test(double)
    test(0, 0.);  // 调用非模板函数void test(int, double)
    test(0);      // 调用void test(T)实例化得到的void test(int)
    test(0, 0);   // 调用void test(T, T)的特化版本void test(int, int)
}
```

上述代码中，我们为test函数定义了4个重载版本，包括两个模板函数以及两个非模板函数，此外，我们还定义了两个模板特化函数，下面分别讨论对test函数的四种调用情况：

1. test(0.)

对于此调用，候选函数包括：

- void test(T)模板函数（其有能力实例化出一个精确匹配的void test(double)，**但这不于重载确定阶段考虑**）
- void test(double)非模板函数

根据上文“精确匹配的非模板函数的优先级大于有能力通过实例化得到精确匹配的模板函数”这一规则，虽然void test(T)模板函数能够实例化出一个精确匹配的void test(double)函数，但由于存在一个精确匹配的非模板函数，故编译器将选择此非模板函数。

2. test(0, 0.)

与test(0.)的情况类似，虽然模板函数void test(T, T)能够实例化出一个精确匹配的void test(int, double)版本，但由于存在一个精确匹配的非模板函数函数，编译器将选择此版本。

3. test(0)

对于此调用，候选函数包括：

- void test(T)模板函数，其有能力实例化出精确匹配版本void test(int)
- 不精确匹配的非模板函数void test(double)

此时，由于通过模板实例化出的void test(int)是唯一精确匹配版本，故编译器将选择此版本。

4. test(0, 0)

与test(0)的情况类似，由于此时不存在精确匹配的非模板函数，则编译器将选择通过模板实例化得到的版本。此外，由于模板存在对于此调用的精确匹配的模板特化版本，所以编译器最终选择了此特化版本。

接下来讨论上述第二点规则，参考以下代码：

``` Cpp
template <typename T>
void test(T) {}

// 一个普适性更低的模板函数
template <typename T>
void test(T *) {}

int main()
{
    test(0);  // 调用void test(T)实例化得到的void test(int)版本
    test(static_cast<void *>(0));  // 调用void test(T *)实例化得到的void test(void *)版本
}
```

上述代码中，void test(T *)较之void test(T)是一个普适性更低的模板函数（类似于基于附加类型的类模板偏特化），void test(T)可接受一切类型的参数，而void test(T *)只能接受一切类型的指针参数。

当我们调用test(0)时，void test(T)为其唯一可行的模板函数，故编译器将选择此模板实例化得到的void test(int)版本进行调用。而对于test(static_cast\<void *\>(0))调用，虽然void test(T)与void test(T *)都是其可行版本，但由于void test(T *)版本的普适性更低，故编译器将选择此版本进行实例化。

樱雨楼

2019.8 于苏州
