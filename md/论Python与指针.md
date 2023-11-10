# 论Python与指针

## 0 引言

指针（Pointer）是C、C++以及Java、Go等语言的一个非常核心且重要的概念，而引用（Reference）是在指针的基础上构建出的一个同样重要的概念。

指针对于任何一个编程语言而言都是必须且重要的，虽然Python对指针这一概念进行了刻意的模糊与限制，但指针对于Python而言依然是一个必须进行深入讨论的话题。

本文基于C++与Python，讨论了Python中与指针及引用相关的一些行为。

## 1 什么是指针？我们为什么需要指针？

指针有两种含义：

1. 指代某种数据类型的指针类型，如整形指针类型，指针指针类型

2. 指代一类存放有内存地址的变量，即指针变量

指针的这两种含义是缺一不可的：作为一种变量，通过指针可以获取某个内存地址，从而为访问此地址上的值做好了准备；作为一种类型，其决定了内存地址的正确偏移长度，其应等于当前类型的单位内存大小。如果一个指针缺少指针类型，即void *，则显然，其虽然保存了内存地址，但这仅仅是一个起点地址，指针会因为无法获知从起点向后进行的偏移量从而拒绝解指针操作；而如果一个指针缺少地址，即nullptr，则其根本无法读取特定位置的内存。

指针存在的意义主要有以下几点：

1. 承载通过malloc、new、allocator等获取的动态内存

2. 使得pass-by-pointer成为可能

pass-by-pointer的好处包括但不限于以下几点：

1. 避免对实参无意义的值拷贝，大幅提高效率

2. 使得对某个变量的修改能力不局限于变量自身作用域

3. 使得swap、移动构造函数、移动赋值运算等操作可以仅针对数据结构内部的指针进行操作，从而避免了对临时对象、移后源等对象的整体内存操作

由此可见，与指针相关的各操作对于编程而言都是必须的或十分重要的。

## 2 C++中的引用

在C++中，引用具有与指针相似的性质，但更加隐形与严格。C++的引用分为以下两种：

### 2.1 左值引用

左值引用于其初始化阶段绑定到左值，且不存在重新绑定。左值引用具有与被绑定左值几乎一样的性质，其唯一的区别在于decltype声明：

``` Cpp
int numA = 0, &lrefA = numA;  // Binding an lvalue
cout << ++lrefA << endl;      // Use the lvalue reference as lvalue & rvalue
decltype(lrefA) numB = 1;     // Error!
```

左值引用常用于pass-by-reference：

``` Cpp
void swap(int &numA, int &numB)
{
    int tmpNum = numA;
    numA = numB;
    numB = tmpNum;
}

int main()
{
    int numA = 1, numB = 2;
    swap(numA, numB);
    cout << numA << endl << numB << endl;  // 2 1
}
```

### 2.2 右值引用

右值引用于其初始化阶段绑定到右值，其常用于移动构造函数和移动赋值操作。在这些场合中，移动构造函数和移动赋值操作通过右值引用接管被移动对象。

右值引用与本文内容无关，故这里不再详述。

## 3 Python中的引用

### 3.1 Python不存在引用

由上文讨论可知，虽然"引用"对于Python而言是一个非常常用的术语，但这显然是不准确的。由于Python不存在（对左/右值的）绑定操作，故不存在左值引用，更不存在右值引用。此外，术语"可变对象（mutable object）"与"不可变对象（immutable object）"也是有失准确的。

### 3.2 Python的指针操作

不难发现，虽然Python没有引用，但其变量的行为和指针的行为具有高度的相似性，这主要体现在以下方面：

1. 在任何情况下（包括赋值、实参传递等）均不存在显式值拷贝，当此种情况发生时，只增加了一次引用计数

2. 变量可以进行重绑定（对应于一个不含顶层const（top-level const）的指针）

3. 在某些情况下（下文将对此问题进行详细讨论），可通过函数实参修改原值

由此可见，Python变量更类似于（某种残缺的）指针变量，而不是引用变量。

#### 3.2.1 构造函数返回指针

对于Python的描述中有一句非常常见的话："一切皆对象"。但在这句话中，有一个很重要的事实常常被人们忽略：对象是一个值，不是一个指针或引用。所以，这句话的准确描述应该更正为："一切皆（某种残缺的）指针"，虽然修改后的描述很抽象，但这是更准确的。

而由于对象从构造函数而来，至此我们可知：Python的构造函数将构造匿名对象，且返回此对象的一个指针。这是Python与指针的第一个重要联系。

代码描述如下：

对于Python代码：

``` Python
sampleNum = 0
```

其不类似于C++代码：

``` Cpp
int sampleNum = 0;
```

而更类似于：

``` Cpp
int __tmpNum = 0, *sampleNum = &__tmpNum;
```

或：

``` Cpp
shared_ptr<int> sampleNum(new int(0));
```

#### 3.2.2 __setitems__操作将隐式解指针

Python与指针的另一个重要联系在于Python的隐式解指针行为。虽然Python不存在显式解指针操作，但（有且仅有）__setitems__操作将进行隐式解指针，通过此方法对变量进行修改等同于通过解指针操作修改变量原值。此种性质意味着：

1. 任何不涉及__setitems__的操作都将成为指针重绑定

对于Python代码：

``` Python
numList = [None] * 10

# Rebinding
numList = [None] * 5
```

其相当于：

``` Cpp
int *numList = new int[10];

// Rebinding
delete[] numList;
numList = new int[5];

delete[] numList;
```

由此可见，对numList的非__setitems__操作导致numList被绑定到了一个新指针上。

2. 任何涉及__setitems__的操作都将成为解指针操作

由于Python对哈希表的高度依赖，"涉及__setitems__的操作"在Python中实际上是一个非常广泛的行为，这主要包括：

（1）对数组的索引操作

（2）对哈希表的查找操作

（3）涉及__setattr__的操作（由于Python将attribute存储在哈希表中，所以__setattr__操作最终将是某种__setitems__操作）

我们用一个稍复杂的例子说明这一点：

对于以下Python代码：

``` Python
class Complex(object):

    def __init__(self, real = 0., imag = 0.):

        self.real = real
        self.imag = imag


    def __repr__(self):

        return '(%.2f, %.2f)' % (self.real, self.imag)


def main():

    complexObj = Complex(1., 2.)

    complexObj.real += 1
    complexObj.imag += 1

    # (2.00, 3.00)
    print(complexObj)


if __name__ == '__main__':
    main()
```

其相当于：

``` Cpp
class Complex
{
public:
    double real, imag;
    Complex(double _real = 0., double _imag = 0.): real(_real), imag(_imag) {}
};

ostream &operator<<(ostream &os, const Complex &complexObj)
{
    return os << "(" << complexObj.real << ", " << complexObj.imag << ")";
}

int main()
{
    Complex *complexObj = new Complex(1., 2.);

    complexObj->real++;
    complexObj->imag++;

    cout << *complexObj << endl;

    delete complexObj;

    return 0;
}
```

由此可见，无论是int，float这种简单的Python类型，还是我们自定义的类，其构造行为都类似使用new构造对象并返回指针。且在Python中任何涉及"."和"[]"的操作，都类似于对指针的"->"或"*"解指针操作。

## 4 后记

本文探讨了Python变量与指针、引用两大概念之间的关系，主要论证了"Python不存在引用"以及"Python变量的行为类似于某种残缺的指针"两个论点。
所有论点均系作者个人观点，如有错误，恭迎指正。

樱雨楼

2019.7 于苏州
