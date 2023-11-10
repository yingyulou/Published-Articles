# C++中的类型转换（下）——强制类型转换

在上篇与中篇中，我们讨论了隐式类型转换及其与函数重载之间的相关话题。本篇将要讨论的即为类型转换的另一大分支——强制类型转换。

## 9. C风格的强制类型转换

在C语言中，强制类型转换存在两种等价形式：Type(Value)或(Type)Value。

参考以下代码：

``` Cpp
int main()
{
    (int *) malloc(0);  // (Type)Value形式的强制类型转换
    int(0.);            // Type(Value)形式的强制类型转换
}
```

上述代码中，我们分别使用了C语言的提供的两种强制类型转换的等价形式将void *转为了int *，以及将double转为了int。

## 10. static_cast

在C++中，static_cast相当于C语言中的强制类型转换语法。static_cast用于在编译期对某种类型的变量进行强制类型转换。

参考以下代码：

``` Cpp
int main()
{
    static_cast<int *>(malloc(0));
    static_cast<int>(0.);
}
```

上述代码中，我们使用了static_cast分别将void *转为了int *，以及将double转为了int。

## 11. const_cast

const_cast是C++中专用于处理与const相关的强制类型转换关键字，其功能为：为一个变量重新设定其const描述。即：const_cast可以为一个变量强行增加或删除其const限定。

**需要明确的是，即使用户通过const_cast强行去除了const属性，也不代表当前变量从不可变变为了可变。const_cast只是使得用户接管了编译器对于const限定的管理权，故用户必须遵守"不修改变量"的承诺。如果违反此承诺，编译器也不会因此而引发编译时错误，但可能引发运行时错误。**

下面讨论const_cast的主要用途。

考察以下代码：

``` Cpp
struct A { A &test() { return *this; } };

int main()
{
    A().test();
}
```

这段代码看上去运行正常。但如果：

``` Cpp
struct A { A &test() { return *this; } };

int main()
{
    const A a;
    a.test();  // Error!
}
```

我们试图用一个const对象去调用非const成员函数，此时，为了调用此成员函数，const A *this就需要转换为A *this，这显然是不行的。

经过上述讨论，我们可以将代码修改为如下：

``` Cpp
struct A { const A &test() const { return *this; } };

int main()
{
    const A a;
    a.test();
}
```

我们将this指针声明为const A *，解决了此问题。但不难发现，如果我们通过一个非const对象调用此方法，其返回值也会被转为const，从而不再可以继续调用任何接受A *this的成员函数。这明显不是我们想要的结果：

``` Cpp
struct A
{
    const A &test() const { return *this; }
    A &test2() { return *this; }
};

int main()
{
    A().test().test2();  // Error!
}
```

怎么解决此问题呢？根据C++函数重载的规则，我们可以为test成员函数同时定义const与非const版本：

``` Cpp
struct A
{
    A &test() { return *this; }
    const A &test() const { return *this; }
    A &test2() { return *this; }
};

int main()
{
    A().test().test2();

    const A a;
    a.test();
}
```

对于A的非const实例而言，test的非const版本是精确匹配，故编译器将选择此版本，从而返回一个A &；同时，对于A的const实例而言，const版本的test是其唯一可用的版本，返回一个const A &。

至此，问题解决了。我们基于const的有无重载出了两个版本的成员函数，从而使得const对象与非const对象能够各自调用不同的版本，互不影响。

在实际情况中，我们定义的两个版本的重载函数除了有无const以外往往没有任何区别，此时就可以使用const_cast定义第二个重载版本，而无需写两遍一模一样的函数体。

参考以下代码：

``` Cpp
struct A
{
    A &test() { ... }

    // 通过const_cast强行去除this的const限定后调用非const版本
    // 返回值通过隐式类型转换再转回const A &
    const A &test() const { return const_cast<A *>(this)->test(); }
};
```

上述代码中，我们首先定义了一个非const版本的test成员函数，这个成员函数将提供给A *this调用；在定义test成员函数的const版本时，我们通过const_cast\<A *\>(this)，将此版本的const A *this指针转换为非const版本需要的A *this类型指针，然后调用了非const版本的test成员函数，并返回其调用结果，非const版本的test成员函数的返回值将通过隐式类型转换转为const A &。

由此可见，通过const_cast，我们仅需一行代码就可以完成第二个函数重载版本的定义。

## 12. dynamic_cast

上文提到，动态类型为继承类的指针或引用可以存储在静态类型为基类的变量中，且不会发生隐式类型转换。对于一个变量而言，虽然其动态类型确实是继承类，但由于编译期与运行期的差别，其也无法跨越"可使用的成员名称由静态类型决定"这一规则。虽然继承类可以通过虚函数的方式一定程度上解决此种情况，但是，如果某个成员函数不是基类虚函数，而只存在于继承类中呢？dynamic_cast为我们提供了解决方案。

当一个静态类型为基类指针或引用的变量确实存放了继承类指针或引用时，从基类向继承类的类型转换，即向下类型转换理论上是可行的，dynamic_cast即用于在运行时实现向下类型转换。需要注意的是，dynamic_cast的使用必须同时满足以下所有条件：

1. 被转换的变量的类型为基类指针或引用，且其确实存放了一个继承类指针或引用
2. 基类具有虚表，即基类必须至少定义了一个虚函数

参考以下代码：

``` Cpp
struct A { virtual void test() {} };  // 基类含有虚函数
struct B: A { void test2() {} };      // 继承类特有函数

int main()
{
    // 静态类型为基类指针的变量存放继承类指针
    A *b = new B;

    // 通过向下类型转换调用继承类特有函数
    dynamic_cast<B *>(b)->test2();
}
```

上述代码中，我们首先定义了具有虚函数的基类A，然后定义了具有继承类特有函数的类B。此时，由于test2成员函数并未在基类中注册为虚函数，我们将无法通过静态类型为A *的变量b调用此函数。但由于我们可以确定变量b的动态类型为B *，则可以于运行时通过dynamic_cast将变量b的静态类型转为B *，然后调用继承类的特有函数test2。

## 13. reinterpret_cast

reinterpret，即"重新解释"，顾名思义，这个强制类型转换的作用是提供某个变量在底层数据上的重新解释。当我们对一个变量使用reinterpret_cast后，编译器将无视任何不合理行为，强行将被转换变量的内存数据重解释为某个新的类型。需要注意的是，reinterpret_cast要求转换前后的类型所占用内存大小一致，否则将引发编译时错误。

参考以下代码：

``` Cpp
int main()
{
    reinterpret_cast<int *>(0);  // 强行将一个整数的内存数据解释为一个int *
}
```

## 14. 讨论

编程语言的强类型与弱类型相关话题，多年来业界一直讨论不休，有的语言发展出了高度弱类型的语法体系，而有的语言则相对严谨，要求用户尽可能多的使用显式类型转换。C++作为一门经典的弱类型语言，其类型转换的相关话题自然十分庞大。

纵观C++的类型转换语法体系，其延续了C++一贯的包罗万象风格，不仅为用户提供了自定义类型转换的极大自由度，也在语法层面为类型转换可能会带来的各种错综复杂的情况作出了严谨的规定。保守看来，如果对C++的类型转换没有深入的理解，或不希望大量使用隐式类型转换时，我们不应过度的依赖诸如非explicit转换构造函数，自定义的类型转换操作符，以及涉及隐式类型转换的各种重载确定等语法组分。但作为C++语法体系的一个重要部分，深入理解C++关于类型转换的各种话题，必定是十分重要的。

樱雨楼

2019.8 于苏州
