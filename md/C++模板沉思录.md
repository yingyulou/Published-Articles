# C++模板沉思录

## 0 论抽象——前言

故事要从一个看起来非常简单的功能开始：

```
请计算两个数的和。
```

如果你对Python很熟悉，你一定会觉得："哇！这太简单了！"，然后写出以下代码：

``` Python
def Plus(lhs, rhs):

    return lhs + rhs
```

那么，C语言又如何呢？你需要面对这样的问题：

``` Cpp
/* 这里写什么？*/ Plus(/* 这里写什么？*/ lhs, /* 这里写什么？*/ rhs)
{
    return lhs + rhs;
}
```

也许你很快就能想到以下解法中的一些或全部：

1. 硬编码为某个特定类型：

``` Cpp
int Plus(int lhs, int rhs)
{
    return lhs + rhs;
}
```

显然，这不是一个好的方案。因为这样的Plus函数接口强行的要求两个实参以及返回值的类型都必须是int，或是能够发生隐式类型转换到int的类型。此时，如果实参并不是int类型，其结果往往就是错误的。请看以下示例：

``` Cpp
int main()
{
    printf("%d\n", Plus(1, 2));          // 3，正确
    printf("%d\n", Plus(1.999, 2.999));  // 仍然是3！
}
```

2. 针对不同类型，定义多个函数

``` Cpp
int Plusi(int lhs, int rhs)
{
    return lhs + rhs;
}


long Plusl(long lhs, long rhs)
{
    return lhs + rhs;
}


double Plusd(double lhs, double rhs)
{
    return lhs + rhs;
}


// ...
```

这种方案的缺点也很明显：其使得代码写起来像"汇编语言"（movl，movq，...）。我们需要针对不同的类型调用不同名称的函数（是的，C语言也不支持函数重载），这太可怕了。

3. 使用宏

``` Cpp
#define Plus(lhs, rhs) (lhs + rhs)
```

这种方案似乎很不错，甚至"代码看上去和Python一样"。但正如许许多多的书籍都讨论过的那样，宏，不仅"抛弃"了类型，甚至"抛弃"了代码。是的，宏不是C语言代码，其只是交付于预处理器执行的"复制粘贴"的标记。一旦预处理完成，宏已然不再存在。可想而知，在功能变得复杂后，宏的缺点将会越来越大：代码晦涩，无法调试，"莫名其妙"的报错...

看到这里，也许你会觉得："哇！C语言真烂！居然连这么简单的功能都无法实现！"。但请想一想，为什么会出现这些问题呢？让我们回到故事的起点：

```
请计算两个数的和。
```

仔细分析这句话："请计算...的和"，意味着"加法"语义，这在C语言中可以通过"+"实现（也许你会联想到汇编语言中的加法实现）；而"两个"，则意味着形参的数量是2（也许你会联想到汇编语言中的ESS、ESP、EBP等寄存器）；那么，"数"，意味着什么语义？C语言中，具有"数"这一语义的类型有十几种：int、double、unsigned，等等，甚至char也具有"数"的语义。那么，"加法"和"+"，"两个"和"形参的数量是2"，以及"数"和int、double、unsigned等等之间的关系是什么？

是抽象。

高级语言的目的，就是对比其更加低级的语言进行抽象，从而使得我们能够实现更加高级的功能。抽象，是一种人类的高级思维活动，是一种充满着智慧的思维活动。汇编语言抽象了机器语言，而C语言则进一步抽象了汇编语言：其将汇编语言中的各种加法指令，抽象成了一个简单的加号；将各种寄存器操作，抽象成了形参和实参...抽象思维是如此的普遍与自然，以至于我们往往甚至忽略了这种思维的存在。

但是，C语言并没有针对类型进行抽象的能力，C语言不知道，也没有能力表达"int和double都是数字"这一语义。而这，直接导致了这个"看起来非常简单的功能"难以完美的实现。

针对类型的抽象是如此重要，以至于编程语言世界出现了与C语言这样的"静态类型语言"完全不一样的"动态类型语言"。正如开头所示，在Python这样的动态类型语言中，我们根本就不需要为每个变量提供类型，从而似乎"从根本上解决了问题"。但是，"出来混，迟早要还的"，这种看似完美的动态类型语言，牺牲的却是极大的运行时效率！我们不禁陷入了沉思：真的没有既不损失效率，又能对类型进行抽象的方案了吗？

正当我们一筹莫展，甚至感到些许绝望之时，C++的模板，为我们照亮了前行的道路。

## 1 新手村——模板基础

### 1.1 函数模板与类模板

模板，即C++中用以实现泛型编程思想的语法组分。模板是什么？一言以蔽之：类型也可以是"变量"的东西。这样的"东西"，在C++中有二：函数模板和类模板。

通过在普通的函数定义和类定义中前置template \<...\>，即可定义一个模板，让我们以上文中的Plus函数进行说明。请看以下示例：

此为函数模板：

``` Cpp
template <typename T>
T Plus(T lhs, T rhs)
{
    return lhs + rhs;
}


int main()
{
    cout << Plus(1, 2) << endl;          // 3，正确！
    cout << Plus(1.999, 2.999) << endl;  // 4.998，同样正确！
}
```

此为类模板：

``` Cpp
template <typename T>
struct Plus
{
    T operator()(T lhs, T rhs)
    {
        return lhs + rhs;
    }
};


int main()
{
    cout << Plus<int>()(1, 2) << endl;             // 3，正确！
    cout << Plus<double>()(1.999, 2.999) << endl;  // 4.998，同样正确！
}
```

显然，模板的出现，使得我们轻而易举的就实现了类型抽象，并且没有（像动态类型语言那样）引入任何因为此种抽象带来的额外代价。

### 1.2 模板形参、模板实参与默认值

请看以下示例：

``` Cpp
template <typename T>
struct Plus
{
    T operator()(T lhs, T rhs)
    {
        return lhs + rhs;
    }
};


int main()
{
    cout << Plus<int>()(1, 2) << endl;
    cout << Plus<double>()(1.999, 2.999) << endl;
}
```

上例中，typename T中的T，称为模板形参；而Plus\<int\>中的int，则称为模板实参。在这里，模板实参是一个类型。

事实上，模板的形参与实参既可以是类型，也可以是值，甚至可以是"模板的模板"；并且，模板形参也可以具有默认值（就和函数形参一样）。请看以下示例：

``` Cpp
template <typename T, int N, template <typename U, typename = allocator<U>> class Container = vector>
class MyArray
{
    Container<T> __data[N];
};


int main()
{
    MyArray<int, 3> _;
}
```

上例中，我们声明了三个模板参数：

1. typename T：一个普通的类型参数
2. int N：一个整型参数
3. template \<typename U, typename = allocator\<U\>\> class Container = vector：一个"模板的模板参数"

什么叫"模板的模板参数"？这里需要明确的是：模板、类型和值，是三个完全不一样的语法组分。模板能够"创造"类型，而类型能够"创造"值。请参考以下示例以进行辨析：

``` Cpp
vector<int> v;
```

此例中，vector是一个模板，vector\<int\>是一个类型，而v是一个值。

所以，一个"模板的模板参数"，就是一个需要提供给其一个模板作为实参的参数。对于上文中的声明，Container是一个"模板的模板参数"，其需要接受一个模板作为实参 。需要怎样的模板呢？这个模板应具有两个模板形参，且第二形参具有默认值allocator\<U\>；同时，Container具有默认值vector，这正是一个符合要求的模板。这样，Container在类定义中，便可被当作一个模板使用（就像vector那样）。

### 1.3 特化与偏特化

模板，代表了一种泛化的语义。显然，既然有泛化语义，就应当有特化语义。特化，使得我们能为某些特定的类型专门提供一份特殊实现，以达到某些目的。

特化分为全特化与偏特化。所谓全特化，即一个"披着空空如也的template \<\>的普通函数或类"，我们还是以上文中的Plus函数为例：

``` Cpp
// 不管T是什么类型，都将使用此定义...
template <typename T>
T Plus(T lhs, T rhs)
{
    return lhs + rhs;
}


// ...但是，当T为int时，将使用此定义
template <>  // 空空如也的template <>
int Plus(int lhs, int rhs)
{
    return lhs + rhs;
}


int main()
{
    Plus(1., 2.);  // 使用泛型版本
    Plus(1, 2);    // 使用特化版本
}
```

那么，偏特化又是什么呢？除了全特化以外的特化，都称为偏特化。这句话虽然简短，但意味深长，让我们来仔细分析一下：首先，"除了全特化以外的..."，代表了template关键词之后的"\<\>"不能为空，否则就是全特化，这显而易见；其次，"...的特化"，代表了偏特化也必须是一个特化。什么叫"是一个特化"呢？只要特化版本比泛型版本更特殊，那么此版本就是一个特化版本。请看以下示例：

``` Cpp
// 泛化版本
template <typename T, typename U>
struct _ {};


// 这个版本的特殊之处在于：仅当两个类型一样的时候，才会且一定会使用此版本
template <typename T>
struct _<T, T> {};


// 这个版本的特殊之处在于：仅当两个类型都是指针的时候，才会且一定会使用此版本
template <typename T, typename U>
struct _<T *, U *> {};


// 这个版本"换汤不换药"，没有任何特别之处，所以不是一个特化，而是错误的重复定义
template <typename A, typename B>
struct _<A, B> {};
```

由此可见，"更特殊"是一个十分宽泛的语义，这赋予了模板极大的表意能力，我们将在下面的章节中不断的见到特化所带来的各种技巧。

### 1.4 惰性实例化

函数模板不是函数，而是一个可以生成函数的语法组分；同理，类模板也不是类，而是一个可以生成类的语法组分。我们称通过函数模板生成函数，或通过类模板生成类的过程为模板实例化。

模板实例化具有一个非常重要的特征：惰性。这种惰性主要体现在类模板上。请看以下示例：

``` Cpp
template <typename T>
struct Test
{
    void Plus(const T &val)  { val + val; }
    void Minus(const T &val) { val - val; }
};


int main()
{
    Test<string>().Plus("abc");
    Test<int>().Minus(0);
}
```

上例中，Minus函数显然是不适用于string类型的。也就是说，Test类对于string类型而言，并不是"100%完美的"。当遇到这种情况时，C++的做法十分宽松：不完美？不要紧，只要不调用那些"不完美的函数"就行了。在编译器层面，编译器只会实例化真的被使用的函数，并对其进行语法检查，而根本不会在意那些根本没有被用到的函数。也就是说，在上例中，编译器实际上只实例化出了两个函数：string版本的Plus，以及int版本的Minus。

在这里，"懒惰即美德"占了上风。

### 1.5 依赖型名称

在C++中，"::"表达"取得"语义。显然，"::"既可以取得一个值，也可以取得一个类型。这在非模板场景下是没有任何问题的，并不会引起接下来即将将要讨论的"取得的是一个类型还是一个值"的语义混淆，因为编译器知道"::"左边的语法组分的定义。但在模板中，如果"::"左边的语法组分并不是一个确切类型，而是一个模板参数的话，语义将不再是确定的。请看以下示例：

``` Cpp
struct A { typedef int TypeOrValue; };
struct B { static constexpr int TypeOrValue = 0; };


template <typename T>
struct C
{
    T::TypeOrValue;  // 这是什么？
};
```

上例中，如果T是A，则T::TypeOrValue是一个类型；而如果T是B，则T::TypeOrValue是一个数。我们称这种含有模板参数的，无法立即确定语义的名称为"依赖型名称"。所谓"依赖"，意即此名称的确切语义依赖于模板参数的实际类型。

对于依赖型名称，C++规定：默认情况下，编译器应认为依赖型名称不是一个类型；如果需要编译器将依赖型名称视为一个类型，则需要前置typename关键词。请看以下示例以进行辨析：

``` Cpp
T::TypeOrValue * N;           // T::TypeOrValue是一个值，这是一个乘法表达式
typename T::TypeOrValue * N;  // typename T::TypeOrValue是一个类型，声明了一个这样类型的指针
```

### 1.6 可变参数模板

可变参数模板是C++11引入的一个极为重要的语法。这里对其进行简要介绍。

可变参数模板表达了"参数数量，以及每个参数的类型都未知且各不相同"这一语义。如果我们希望实现一个简单的print函数，其能够传入任意数量，且类型互不相同的参数，并依次打印这些参数值，此时就需要使用可变参数模板。

可变参数模板的语法由以下组分构成：

1. typename...：声明一个可变参数模板形参
2. sizeof...：获取参数包内参数的数量
3. Pattern...：以某一模式展开参数包

接下来，我们就基于可变参数模板，实现这一print函数。请看以下示例：

``` Cpp
// 递归终点
void print() {}


// 分解出一个val + 剩下的所有val
// 相当于：void print(const T &val, const Types1 &Args1, const Types2 &Args2, const Types3 &Args3, ...)
template <typename T, typename... Types>
void print(const T &val, const Types &... Args)
{
    // 每次打印一个val
    cout << val << endl;

    // 相当于：print(Args1, Args2, Args3, ...);
    // 递归地继续分解...
    print(Args...);
}


int main()
{
    print(1, 2., '3', "4");
}
```

上例中，我们实现了一对重载的print函数。第一个print函数是一个空函数，其将在"Args..."是空的时候被调用，以作为递归终点；而第二个print函数接受一个val以及余下的所有val作为参数，其将打印val，并使用余下的所有val继续递归调用自己。不难发现，第二版本的print函数具有不断打印并分解Args的能力，直到Args被完全分解。

## 2 平淡无奇却暗藏玄机的语法——sizeof与SFINAE

### 2.1 sizeof

"sizeof？这有什么可讨论的？"也许你会想。只要你学过C语言，那么对此必不陌生。那么为什么我们还需要为sizeof这一"平淡无奇"的语法单独安排一节来讨论呢？这是因为sizeof有两个对于泛型编程而言极为重要的特性：

1. sizeof的求值结果是编译期常量（从而可以作为模板实参使用）
2. 在任何情况下，sizeof都不会引发对其参数的求值或类似行为（如函数调用，甚至函数定义！等），因为并不需要

上述第一点很好理解，因为sizeof所考察的是类型，而类型（当然也包含其所占用的内存大小），一定是一个编译期就知道的量（因为C++作为一门静态类型语言，任何的类型都绝不会延迟到运行时才知道，这是动态类型语言才具有的特性），故sizeof的结果是一个编译期常量也就不足为奇了。

上述第二点意味深长。利用此特性，我们可以实现出一些非常特殊的功能。请看下一节。

### 2.2 稻草人函数

让我们以一个问题引出这一节的内容：

```
如何实现：判定类型A是否能够基于隐式类型转换转为B类型？
```

乍看之下，这是个十分棘手的问题。此时我们应当思考的是：如何引导（请注意"引导"一词的含义）编译器，在A到B的隐式类型转换可行时，走第一条路，否则，走第二条路？

请看以下示例：

``` Cpp
template <typename A, typename B>
class IsCastable
{
private:

    // 定义两个内存大小不一样的类型，作为"布尔值"
    typedef char __True;
    typedef struct { char _[2]; } __False;


    // 稻草人函数
    static A __A();


    // 只要A到B的隐式类型转换可用，重载确定的结果就是此函数...
    static __True __Test(B);


    // ...否则，重载确定的结果才是此函数("..."参数的重载确定优先级低于其他一切可行的重载版本)
    static __False __Test(...);


public:

    // 根据重载确定的结果，就能够判定出隐式类型转换是否能够发生
    static constexpr bool Value = sizeof(__Test(__A())) == sizeof(__True);
};
```

上例比较复杂，我们依次进行讨论。

首先，我们声明了两个大小不同的类型，作为假想的"布尔值"。也许你会有疑问，这里为什么不使用int或double之类的类型作为False？这是由于C语言并未规定"int、double必须比char大"，故为了"强行满足标准"（你完全可以认为这是某种"教条主义或形式主义"），这里采用了"两个char一定比一个char大一倍"这一简单道理，定义了False。

然后，我们声明了一个所谓的"稻草人函数"，这个看似毫无意义的函数甚至没有函数体（因为并不需要，且接下来的两个函数也没有函数体，与此函数同理）。这个函数唯一的目的就是"获得"一个A类型的值"给sizeof看"。由于sizeof的不求值特性，此函数也就不需要（我们也无法提供）函数体了。那么，为什么不直接使用形如"T\(\)"这样的写法，而需要声明一个"稻草人函数"呢？我想，不用我说你就已经明白原因了：这是因为并不是所有的T都具有默认构造函数，而如果T没有默认构造函数，那么"T\(\)"就是错误的。

接下来是最关键的部分，我们声明了一对重载函数，这两个函数的区别有二：

1. 返回值不同，一个是sizeof的结果为1的值，而另一个是sizeof的结果为2的值
2. 形参不同，一个是B，一个是"..."

也就是说，如果我们给这一对重载函数传入一个A类型的值时，由于"..."参数的重载确定优先级低于其他一切可行的重载版本，只要A到B的隐式类型转换能够发生，重载确定的结果就一定是调用第一个版本的函数，返回值为\_\_True；否则，只有当A到B的隐式类型转换真的不可行时，编译器才会"被迫"选择那个编译器"最不喜欢的版本"，从而使得返回值为\_\_False。返回值的不同，就能够直接体现在sizeof的结果不同上。所以，只需要判定sizeof\(\_\_Test\(\_\_A\(\)\)\)是多少，就能够达到我们最终的目的了。下面请看使用示例：

``` Cpp
int main()
{
    cout << IsCastable<int, double>::Value << endl;  // true
    cout << IsCastable<int, string>::Value << endl;  // false
}
```

可以看出，输出结果完全符合我们的预期。

### 2.3 SFINAE

SFINAE（Substitution Failure Is Not An Error，替换失败并非错误）是一个高级模板技巧。首先，让我们来分析这一拗口的词语："替换失败并非错误"。

什么是"替换"？这里的替换，实际上指的正是模板实例化；也就是说，当模板实例化失败时，编译器并不认为这是一个错误。这句话看上去似乎莫名其妙，也许你会有疑问：那怎么样才认为是一个错误？我们又为什么要讨论一个"错误的东西"呢？让我们以一个问题引出这一技巧的意义：

```
如何判定一个类型是否是一个类类型？
```

"哇！这个问题似乎比上一个问题更难啊！"也许你会这么想。不过有了上一个问题的铺垫，这里我们依然要思考的是：一个类类型，有什么独一无二的东西是非类类型所没有的？（这样我们似乎就能让编译器在"喜欢和不喜欢"之间做出抉择）

也许你将恍然大悟：类的成员指针。

请看以下示例：

``` Cpp
template <typename T>
class IsClass
{
private:

    // 定义两个内存大小不一样的类型，作为"布尔值"
    typedef char __True;
    typedef struct { char _[2]; } __False;


    // 仅当T是一个类类型时，"int T::*"才是存在的，从而这个泛型函数的实例化才是可行的
    // 否则，就将触发SFINAE
    template <typename U>
    static __True __Test(int U::*);


    // 仅当触发SFINAE时，编译器才会"被迫"选择这个版本
    template <typename U>
    static __False __Test(...);


public:

    // 根据重载确定的结果，就能够判定出T是否为类类型
    static constexpr bool Value = sizeof(__Test<T>(0)) == sizeof(__True);
};
```

同样，我们首先定义了两个内存大小一定不一样的类型，作为假想的"布尔值"。然后，我们声明了两个重载模板，其分别以两个"布尔值"作为返回值。这里的关键在于，重载模板的参数，一个是类成员指针，另一个是"..."。显然，当编译器拿到一个T，并准备生成一个"T::\*"时，仅当T是一个类类型时，这一生成才是正确的，合乎语法的；否则，这个函数签名将根本无法被生成出来，从而进一步的使得编译器"被迫"选择那个"最不喜欢的版本"进行调用（而不是认为这个"根本无法被生成出来"的模板是一个错误）。所以，通过sizeof对\_\_Test的返回值大小进行判定，就能够达到我们最终的目的了。下面请看使用示例：

``` Cpp
int main()
{
    cout << IsClass<double>::Value << endl;  // false
    cout << IsClass<string>::Value << endl;  // true
}
```

可以看出，输出结果完全符合我们的预期。

### 2.4 本章后记

sizeof，作为一个C语言的"入门级"语法，其"永不求值"的特性往往被我们所忽略。本章中，我们充分利用了sizeof的这种"永不求值"的特性，做了很多"表面工程"，仅仅是为了"给sizeof看"；同理，SFINAE技术似乎也只是在"找编译器的麻烦，拿编译器寻开心"。但正是这些"表面工程、找麻烦、寻开心"，让我们得以实现了一些非常不可思议的功能。

## 3 类型萃取器——Type Traits

Traits，中文翻译为"特性"，Type Traits，即为"类型的特性"。这是个十分奇怪的翻译，故很多书籍对这个词选择不译，也有书籍将其翻译为"类型萃取器"，十分生动形象。

Type Traits的定义较为模糊，其大致代表了这样的一系列技术：通过一个类型T，取得另一个基于T进行加工后的类型，或对T基于某一标准进行分类，得到分类结果。

本章中，我们以几个经典的Type Traits应用，来见识一番此技术的精妙。

### 3.1 为T"添加星号"

第一个例子较为简单：我们需要得到T的指针类型，即：得到"T \*"。此时，只需要将"T \*"通过typedef变为Type Traits类的结果即可。请看以下示例：

``` Cpp
template <typename T>
struct AddStar { typedef T *Type; };


template <typename T>
struct AddStar<T *> { typedef T *Type; };


int main()
{
    cout << typeid(AddStar<int>::Type).name() << endl;    // int *
    cout << typeid(AddStar<int *>::Type).name() << endl;  // int *
}
```

这段代码十分简单，但似乎我们写了两遍"一模一样"的代码？认真观察和思考即可发现：特化版本是为了防止一个已经是指针的类型发生"升级"而存在的。如果T已经是一个指针类型，则Type就是T本身，否则，Type才是"T \*"。

### 3.2 为T"去除星号"

上一节，我们实现了一个能够为T"添加星号"的Traits，这一节，我们将实现一个功能与之相反的Traits：为T"去除星号"。

"简单！"也许你会想，并很快给出了以下实现：

``` Cpp
template <typename T>
struct RemoveStar { typedef T Type; };


template <typename T>
struct RemoveStar<T *> { typedef T Type; };


int main()
{
    cout << typeid(RemoveStar<int>::Type).name() << endl;    // int
    cout << typeid(RemoveStar<int *>::Type).name() << endl;  // int
}
```

似乎完成了？不幸的是，这一实现并不完美。请看以下示例：

``` Cpp
int main()
{
    cout << typeid(RemoveStar<int **>::Type).name() << endl;  // int *，哦不！
}
```

可以看到，我们的上述实现只能去除一个星号，当传入一个多级指针时，并不能得到我们想要的结果。

这该如何是好？我们不禁想到：如果能够实现一个"while循环"，就能去除所有的星号了。虽然模板没有while循环，但我们知道：递归正是循环的等价形式。请看以下示例：

``` Cpp
// 递归终点，此时T真的不是指针了
template <typename T>
struct RemoveStar { typedef T Type; };


// 当T是指针时，Type应该是T本身（已经去除了一个星号）继续RemoveStar的结果
template <typename T>
struct RemoveStar<T *> { typedef typename RemoveStar<T>::Type Type; };
```

上述实现中，当发现T选择了特化版本（即T本身是指针时），就会递归地对T进行去星号，直到T不再选择特化版本，从而抵达递归终点为止。这样，就能在面对多级指针时，也能够得到正确的Type。下面请看使用示例：

``` Cpp
int main()
{
    cout << typeid(RemoveStar<int **********>::Type).name() << endl;  // int
}
```

可以看出，输出结果完全符合我们的预期。

显然，使用这样的Traits是具有潜在的较大代价的。例如上例中，为了去除一个十级指针的星号，编译器竟然需要实例化出11个类！但好在这一切均发生在编译期，对运行效率不会产生任何影响。

### 3.3 寻找"最强大类型"

让我们继续讨论前言中的Plus函数，以引出本节所要讨论的话题。目前我们给出的"最好实现"如下：

``` Cpp
template <typename T>
T Plus(T lhs, T rhs)
{
    return lhs + rhs;
}


int main()
{
    cout << Plus(1, 2) << endl;  // 3，正确！
}
```

但是，只要在上述代码中添加一个"."，就立即发生了问题：

``` Cpp
int main()
{
    cout << Plus(1, 2.) << endl;  // 二义性错误！T应该是int还是double？
}
```

上例中，由于Plus模板只使用了单一的一个模板参数，故要求两个实参的类型必须一致，否则，编译器就不知道T应该是什么类型，从而引发二义性错误。但显然，任何的两种"数"之间都应该是可以做加法的，所以不难想到，我们应该使用两个而不是一个模板参数，分别作为lhs与rhs的类型，但是，我们立即就遇到了新的问题。请看以下示例：

``` Cpp
template <typename T1, typename T2>
/* 这里应该写什么？*/ Plus(T1 lhs, T2 rhs)
{
    return lhs + rhs;
}
```

应该写T1？还是T2？显然都不对。我们应该寻求一种方法，其能够获取到T1与T2之间的"更强大类型"，并将此"更强大类型"作为返回值。进一步的，我们可以以此为基础，实现出一个能够获取到任意数量的类型之中的"最强大类型"的方法。

应该怎么做呢？事实上，这个问题的解决方案，确实是难以想到的。请看以下示例：

``` Cpp
template <typename A, typename B>
class StrongerType
{
private:

    // 稻草人函数
    static A __A();
    static B __B();


public:

    // 3目运算符表达式的类型就是"更强大类型"
    typedef decltype(true ? __A() : __B()) Type;
};


int main()
{
    cout << typeid(StrongerType<int, char>::Type).name() << endl;    // int
    cout << typeid(StrongerType<int, double>::Type).name() << endl;  // double
}
```

上例中，我们首先定义了两个"稻草人函数"，用以分别"获取"类型为A或B的值"给decltype看"。然后，我们使用了decltype探测三目运算符表达式的类型，不难发现，decltype也具有sizeof的"不对表达式进行求值"的特性。由于三目运算符表达式从理论上可能返回两个值中的任意一个，故表达式的类型就是我们所寻求的"更强大类型"。随后的用例也证实了这一点。

有了获取两个类型之间的"更强大类型"的Traits以后，我们不难想到：N个类型之中的"最强大类型"，就是N - 1个类型之中的"最强大类型"与第N个类型之间的"更强大类型"。请看以下示例：

``` Cpp
// 原型
// 通过typename StrongerType<Types...>::Type获取Types...中的"最强大类型"
template <typename... Types>
class StrongerType;


// 只有一个类型
template <typename T>
class StrongerType<T>
{
    // 我自己就是"最强大的"
    typedef T Type;
};


// 只有两个类型
template <typename A, typename B>
class StrongerType<A, B>
{
private:

    // 稻草人函数
    static A __A();
    static B __B();


public:

    // 3目运算符表达式的类型就是"更强大类型"
    typedef decltype(true ? __A() : __B()) Type;
};


// 不止两个类型
template <typename T, typename... Types>
class StrongerType<T, Types...>
{
public:

    // T和typename StrongerType<Types...>::Type之间的"更强大类型"就是"最强大类型"
    typedef typename StrongerType<T, typename StrongerType<Types...>::Type>::Type Type;
};


int main()
{
    cout << typeid(StrongerType<char, int>::Type).name() << endl;          // int
    cout << typeid(StrongerType<int, double>::Type).name() << endl;        // double
    cout << typeid(StrongerType<char, int, double>::Type).name() << endl;  // double
}
```

通过递归，我们使得所有的类型共同参与了"打擂台"，这里的"擂台"，就是我们已经实现了的StrongerType的双类型版本，而"打擂台的最后大赢家"，则正是我们所寻求的"最强大类型"。

有了StrongerType这一Traits后，我们就可以实现上文中的双类型版本的Plus函数了。请看以下示例：

``` Cpp
// Plus函数的返回值应该是T1与T2之间的"更强大类型"
template <typename T1, typename T2>
typename StrongerType<T1, T2>::Type Plus(T1 lhs, T2 rhs)
{
    return lhs + rhs;
}


int main()
{
    Plus(1, 2.);  // 完美！
}
```

至此，我们"终于"实现了一个最完美的Plus函数。

### 3.4 本章后记

本章所实现的三个小工具，都是STL的type\_traits库的一部分。值得一提的是我们最后实现的获取"最强大类型"的工具：这一工具所解决的问题，实际上是一个非常经典的问题，其多次出现在多部著作中。由于decltype（以及可变参数模板）是C++11的产物，故很多较老的书籍对此问题给出了"无解"的结论，或只能给出一些较为牵强的解决方案。

## 4 "压榨"编译器——编译期计算

值也能成为模板参数的一部分，而模板参数是编译期常量，这二者的结合使得通过模板进行（较复杂的）编译期计算成为了可能。由于编译器本就不是"计算器"，故标题中使用了"压榨"一词，以表达此技术的"高昂的编译期代价"以及"较大的局限性"的特点；同时，合理的利用编译期计算技术，能够极大地提高程序的效率，故"压榨"也有"压榨性能"之意。

本章中，我们以一小一大两个示例，来讨论编译期计算这一巧妙技术的应用。

### 4.1 编译期计算阶乘

编译期计算阶乘是编译期计算技术的经典案例，许多书籍对此均有讨论（往往作为"模板元编程"一章的首个案例）。那么首先，让我们来看看一个普通的阶乘函数的实现：

``` Cpp
int Factorial(int N)
{
    return N == 1 ? 1 : N * Factorial(N - 1);
}
```

这个实现很简单，这里就不对其进行详细讨论了。下面，我们来看看如何将这个函数"翻译"为一个编译期就进行计算并得到结果的"函数"。请看以下示例：

``` Cpp
// 递归起点
template <int N>
struct Factorial
{
    static constexpr int Value = N * Factorial<N - 1>::Value;
};


// 递归终点
template <>
struct Factorial<1>
{
    static constexpr int Value = 1;
};


int main()
{
    cout << Factorial<4>::Value;  // 编译期就能获得结果
}
```

观察上述代码，不难总结出我们的"翻译"规则：

1. 形参N（运行时值）变为了模板参数N（编译期值）
2. "N == 1"这样的"if语句"变为了模板特化
3. 递归变为了创造一个新的模板（Factorial\<N - 1\>），这也意味着循环也可以通过此种方式实现
4. "return"变为了一个static constexpr变量

上述四点"翻译"规则几乎就是编译期计算的全部技巧了！接下来，就让我们以一个更复杂的例子来继续讨论这一技术的精彩之处：编译期分数的实现。

### 4.2 编译期分数

分数，由分子和分母组成。有了上一节的铺垫，我们不难发现：分数正是一个可以使用编译期计算技术的极佳场合。所以首先，我们需要实现一个编译期分数类。编译期分数类的实现非常简单，我们只需要通过一个"构造函数"将模板参数保留下来，作为静态数据成员即可。请看以下示例：

``` Cpp
template <long long __Numerator, long long __Denominator>
struct Fraction
{
    // "构造函数"
    static constexpr long long Numerator   = __Numerator;
    static constexpr long long Denominator = __Denominator;


    // 将编译期分数转为编译期浮点数
    template <typename T = double>
    static constexpr T Eval() { return static_cast<T>(Numerator) / static_cast<T>(Denominator); }
};


int main()
{
    // 1/2
    typedef Fraction<1, 2> OneTwo;

    // 0.5
    cout << OneTwo::Eval<>();
}
```

由使用示例可见：编译期分数的"实例化"只需要一个typedef即可；并且，我们也能通过一个编译期分数得到一个编译期浮点数。

让我们继续讨论下一个问题：如何实现约分和通分？

显然，约分和通分需要"求得两个数的最大公约数和最小公倍数"的算法。所以，我们首先来看看这两个算法的"普通"实现：

``` Cpp
// 求得两个数的最大公约数
long long GreatestCommonDivisor(long long lhs, long long rhs)
{
    return rhs == 0 ? lhs : GreatestCommonDivisor(rhs, lhs % rhs);
}


// 求得两个数的最小公倍数
long long LeastCommonMultiple(long long lhs, long long rhs)
{
    return lhs * rhs / GreatestCommonDivisor(lhs, rhs);
}
```

根据上一节的"翻译规则"，我们不难翻译出以下代码：

``` Cpp
// 对应于"return rhs == 0 ? ... : GreatestCommonDivisor(rhs, lhs % rhs)"部分
template <long long LHS, long long RHS>
struct __GreatestCommonDivisor
{
    static constexpr long long __Value = __GreatestCommonDivisor<RHS, LHS % RHS>::__Value;
};


// 对应于"return rhs == 0 ? lhs : ..."部分
template <long long LHS>
struct __GreatestCommonDivisor<LHS, 0>
{
    static constexpr long long __Value = LHS;
};


// 对应于"return lhs * rhs / GreatestCommonDivisor(lhs, rhs)"部分
template <long long LHS, long long RHS>
struct __LeastCommonMultiple
{
    static constexpr long long __Value = LHS * RHS /
        __GreatestCommonDivisor<LHS, RHS>::__Value;
};
```

有了上面的这两个工具，我们就能够实现出通分和约分了。首先，我们可以改进一开始的Fraction类，在"构造函数"中加入"自动约分"功能。请看以下示例：

``` Cpp
template <long long __Numerator, long long __Denominator>
struct Fraction
{
    // 具有"自动约分"功能的"构造函数"
    static constexpr long long Numerator = __Numerator /
        __GreatestCommonDivisor<__Numerator, __Denominator>::__Value;

    static constexpr long long Denominator = __Denominator /
        __GreatestCommonDivisor<__Numerator, __Denominator>::__Value;
};


int main()
{
    // 2/4 => 1/2
    typedef Fraction<2, 4> OneTwo;
}
```

可以看出，我们只需在"构造函数"中添加对分子、分母同时除以其最大公约数的运算，就能够实现"自动约分"了。

接下来，我们来实现分数的四则运算功能。显然，分数的四则运算的结果还是一个分数，故我们只需要通过using，将"四则运算模板"与"等价的结果分数模板"连接起来即可实现。请看以下示例：

``` Cpp
// FractionAdd其实就是一个特殊的编译期分数模板
template <typename LHS, typename RHS>
using FractionAdd = Fraction<

    // 将通分后的分子相加
    LHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__LValue +
    RHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__RValue,

    // 通分后的分母
    __LeastCommonMultiple<LHS::Denominator, RHS::Denominator>::__Value

    // 自动约分
>;


// FractionMinus其实也是一个特殊的编译期分数模板
template <typename LHS, typename RHS>
using FractionMinus = Fraction<

    // 将通分后的分子相减
    LHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__LValue -
    RHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__RValue,

    // 通分后的分母
    __LeastCommonMultiple<LHS::Denominator, RHS::Denominator>::__Value

    // 自动约分
>;


// FractionMultiply其实也是一个特殊的编译期分数模板
template <typename LHS, typename RHS>
using FractionMultiply = Fraction<

    // 分子与分子相乘
    LHS::Numerator * RHS::Numerator,

    // 分母与分母相乘
    LHS::Denominator * RHS::Denominator

    // 自动约分
>;


// FractionDivide其实也是一个特殊的编译期分数模板
template <typename LHS, typename RHS>
using FractionDivide = Fraction<

    // 分子与分母相乘
    LHS::Numerator * RHS::Denominator,

    // 分母与分子相乘
    LHS::Denominator * RHS::Numerator

    // 自动约分
>;


int main()
{
    // 1/2
    typedef Fraction<1, 2> OneTwo;

    // 2/3
    typedef Fraction<2, 3> TwoThree;

    // 2/3 + 1/2 => 7/6
    typedef FractionAdd<TwoThree, OneTwo> TwoThreeAddOneTwo;

    // 2/3 - 1/2 => 1/6
    typedef FractionMinus<TwoThree, OneTwo> TwoThreeMinusOneTwo;

    // 2/3 * 1/2 => 1/3
    typedef FractionMultiply<TwoThree, OneTwo> TwoThreeMultiplyOneTwo;

    // 2/3 / 1/2 => 4/3
    typedef FractionDivide<TwoThree, OneTwo> TwoThreeDivideOneTwo;
}
```

由此可见，所谓的四则运算，实际上就是一个针对Fraction的using（模板不能使用typedef，只能使用using）罢了。

最后，我们实现分数的比大小功能。这非常简单：只需要先对分母通分，再对分子进行比大小即可。而比大小的结果，就是"比大小模板"的一个数据成员。请看以下示例：

``` Cpp
// 这六个模板都进行"先通分，再比较"运算，唯一的区别就在于比较操作符的不同

// "operator=="
template <typename LHS, typename RHS>
struct FractionEqual
{
    static constexpr bool Value =
        LHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__LValue ==
        RHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__RValue;
};


// "operator!="
template <typename LHS, typename RHS>
struct FractionNotEqual
{
    static constexpr bool Value =
        LHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__LValue !=
        RHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__RValue;
};


// "operator<"
template <typename LHS, typename RHS>
struct FractionLess
{
    static constexpr bool Value =
        LHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__LValue <
        RHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__RValue;
};


// "operator<="
template <typename LHS, typename RHS>
struct FractionLessEqual
{
    static constexpr bool Value =
        LHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__LValue <=
        RHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__RValue;
};


// "operator>"
template <typename LHS, typename RHS>
struct FractionGreater
{
    static constexpr bool Value =
        LHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__LValue >
        RHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__RValue;
};


// "operato>="
template <typename LHS, typename RHS>
struct FractionGreaterEqual
{
    static constexpr bool Value =
        LHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__LValue >=
        RHS::Numerator * __CommonPoints<LHS::Denominator, RHS::Denominator>::__RValue;
};


int main()
{
    // 1/2
    typedef Fraction<1, 2> OneTwo;

    // 2/3
    typedef Fraction<2, 3> TwoThree;

    // 1/2 == 2/3 => false
    cout << FractionEqual<OneTwo, TwoThree>::Value << endl;

    // 1/2 != 2/3 => true
    cout << FractionNotEqual<OneTwo, TwoThree>::Value << endl;

    // 1/2 < 2/3 => true
    cout << FractionLess<OneTwo, TwoThree>::Value << endl;

    // 1/2 <= 2/3 => true
    cout << FractionLessEqual<OneTwo, TwoThree>::Value << endl;

    // 1/2 > 2/3 => false
    cout << FractionGreater<OneTwo, TwoThree>::Value << endl;

    // 1/2 >= 2/3 => false
    cout << FractionGreaterEqual<OneTwo, TwoThree>::Value << endl;
}
```

至此，编译期分数的全部功能就都实现完毕了。不难发现，在编译期分数的使用过程中，我们全程使用的都是typedef，并没有真正的构造任何一个分数，一切计算都已经在编译期完成了。

### 4.3 本章后记

读完本章，也许你会恍然大悟："哦！原来模板也能够表达形参、if、while、return等语义！"，进而，也许你会有疑问："那既然这样，岂不是所有的计算函数都能换成编译期计算了？"。

很可惜，答案是否定的。

我们通过对编译期计算这一技术的优缺点进行总结，从而回答这个问题。编译期计算的目的，是为了完全消除运行时代价，从而在高性能计算场合极大的提高效率；但此技术的缺点也是很多且很明显的：首先，仅仅为了进行一次编译期计算，就有可能进行很多次的模板实例化（比如，为了计算10的阶乘，就要实例化出10个Factorial类），这是一种极大的潜在的编译期代价；其次，并不是任何类型的值都能作为模板参数，如浮点数（虽然我们可以使用编译期分数间接的规避这一限制）、以及任何的类类型值等均不可以，这就使得编译期计算的应用几乎被限定在只需要使用整型和布尔类型的场合中；最后，"递归实例化"在所有的编译器中都是有最大深度限制的（不过幸运的是，在现代编译器中，允许的最大深度其实是比较大的）。但即使如此，由于编译期计算技术使得我们可以进行"抢跑"，在程序还未开始运行时，就计算出各种复杂的结果，从而极大的提升程序的效率，故此技术当然也是瑕不掩瑜的。

## 5 神奇的"多功能"函数——编译期分派

本章旨在讨论这样的一个问题：

```
如何实现一个"多功能函数"，使得单一的"多功能函数"在面对不同类型的参数时，能够自动选择针对当前类型的最佳方案或独特功能？
```

本章亦将通过一小一大两个案例，来讨论这一问题。

### 5.1 迭代器的advance函数

STL中的advance函数，可用于将迭代器向前（后）推进N步。显然，不同的迭代器类别对前进这一动作的支持是不一样的：如前向迭代器，只能前进，不能后退；双向迭代器，可双向移动；而随机访问迭代器，不仅可以双向移动，还可以"大跨步地"移动。为了讨论方便，我们首先用数字的加减模拟出这三种不同类别的"迭代器"。请看以下示例：

``` Cpp
// 模拟的"前向迭代器"
template <typename T>
class ForwardIterator
{
public:

    // 构造函数
    ForwardIterator(const T &val);


    // operator*
    T &operator*() { return __val; }


    // 只能++
    void operator++(int) { __val++; }


private:

    // 数据成员
    T __val;
};


// 模拟的"双向迭代器"
template <typename T>
class BidirectionalIterator
{
public:

    // 构造函数
    BidirectionalIterator(const T &val);


    // operator*
    T &operator*() { return __val; }


    // 可以++和--
    void operator++(int) { __val++; }
    void operator--(int) { __val--; }


private:

    // 数据成员
    T __val;
};


// 模拟的"随机访问迭代器"
template <typename T>
class RandomAccessIterator
{
public:

    // 构造函数
    RandomAccessIterator(const T &val);


    // operator*
    T &operator*() { return __val; }


    // 不仅可以++和--，还可以直接+=、-=
    void operator++(int) { __val++; }
    void operator--(int) { __val--; }
    void operator+=(int N) { __val += N; }
    void operator-=(int N) { __val -= N; }


private:

    // 数据成员
    T __val;
};


template <typename T>
ForwardIterator<T>::ForwardIterator(const T &val): __val(val) {}


template <typename T>
BidirectionalIterator<T>::BidirectionalIterator(const T &val): __val(val) {}


template <typename T>
RandomAccessIterator<T>::RandomAccessIterator(const T &val): __val(val) {}
```

此时，我们就可以实现出一个简单的advance函数了。请看以下示例：

``` Cpp
template <typename Iterator>
void Advance(Iterator &iter, int N)
{
    for (int _ = 0; _ < N; _++) iter++;
}


int main()
{
    ForwardIterator<int> forwardIterator(0);
    BidirectionalIterator<int> bidirectionalIterator(0);
    RandomAccessIterator<int> randomAccessIterator(0);

    Advance(forwardIterator, 10);
    Advance(bidirectionalIterator, 10);
    Advance(randomAccessIterator, 10);

    cout << *forwardIterator << endl;        // 10
    cout << *bidirectionalIterator << endl;  // 10
    cout << *randomAccessIterator << endl;   // 10
}
```

上述实现似乎运行正常，但稍加分析不难发现，这一实现具有以下两个主要缺点：

1. N必须为正数，即使对于（允许N为负数的）双向迭代器和随机访问迭代器而言也是一样
2. 随机访问迭代器并不需要像此实现这样"步进"，从而造成了效率的浪费

怎么样才能既提供单一接口，又能够让接口有能力分辨出不同的迭代器类别呢？首先，我们需要将"迭代器的类别"这一语义表达出来，可以通过"强行编造"一些空类实现这一语义。请看以下示例：

``` Cpp
// "强行编造"的三种不同的迭代器类别
struct ForwardIteratorTag {};
struct BidirectionalIteratorTag {};
struct RandomAccessIteratorTag {};
```

然后，我们可以为每个迭代器添加一个typedef，从而表示出这个迭代器的类别。请看以下示例：

``` Cpp
template <typename T>
class ForwardIterator
{
public:

    // 前向迭代器的类别是ForwardIteratorTag
    typedef ForwardIteratorTag IteratorCategory;


    // ...
};


template <typename T>
class BidirectionalIterator
{
public:

    // 双向迭代器的类别是BidirectionalIteratorTag
    typedef BidirectionalIteratorTag IteratorCategory;


    // ...
};


template <typename T>
class RandomAccessIterator
{
public:

    // 随机访问迭代器的类别是RandomAccessIteratorTag
    typedef RandomAccessIteratorTag IteratorCategory;


    // ...
};
```

此时，当我们拿到一个迭代器类型Iterator时，我们就可以通过typename Iterator::IteratorCategory来获取到当前迭代器的类别了。

同时，我们可以以迭代器类别作为Advance函数的第三参数，从而重载出多个不同版本的Advance函数。请看以下示例：

``` Cpp
// 适用于前向迭代器的版本
template <typename Iterator>
void __Advance(Iterator &iter, int N, ForwardIteratorTag)
{
    for (int _ = 0; _ < N; _++) iter++;
}


// 适用于双向迭代器的版本
template <typename Iterator>
void __Advance(Iterator &iter, int N, BidirectionalIteratorTag)
{
    if (N > 0)
    {
        for (int _ = 0; _ < N; _++) iter++;
    }
    else
    {
        for (int _ = 0; _ < -N; _++) iter--;
    }
}


// 适用于随机访问迭代器的版本
template <typename Iterator>
void __Advance(Iterator &iter, int N, RandomAccessIteratorTag)
{
    iter += N;
}
```

此时，我们已经拥有了两组工具：

1. 可以通过typename Iterator::IteratorCategory来获取到当前迭代器的类别
2. 实现了3个\_\_Advance函数，分别适用于三个迭代器类别

此时，只要我们使用迭代器类别（作为第三参数）去调用\_\_Advance函数，编译器就将根据重载确定规则，选择适用于当前迭代器类别的\_\_Advance函数进行调用了。请看以下示例：

``` Cpp
int main()
{
    ForwardIterator<int> forwardIterator(0);
    BidirectionalIterator<int> bidirectionalIterator(0);
    RandomAccessIterator<int> randomAccessIterator(0);

    // 重载确定至__Advance(Iterator &iter, int N, ForwardIteratorTag)版本
    __Advance(forwardIterator, 10, typename ForwardIterator<int>::IteratorCategory());

    // 重载确定至__Advance(Iterator &iter, int N, BidirectionalIteratorTag)版本
    __Advance(bidirectionalIterator, 10, typename BidirectionalIterator<int>::IteratorCategory());

    // 重载确定至__Advance(Iterator &iter, int N, RandomAccessIteratorTag)版本
    __Advance(randomAccessIterator, 10, typename RandomAccessIterator<int>::IteratorCategory());

    cout << *forwardIterator << endl;        // 10
    cout << *bidirectionalIterator << endl;  // 10
    cout << *randomAccessIterator << endl;   // 10
}
```

这就结束了吗？怎么感觉函数的调用这么麻烦呢？显然还未结束。

不难发现，在任何一个函数的实现代码中，我们都不仅拥有参数值，还拥有每个参数的类型。所以，我们可以再实现一个只有两个参数的函数，并在其内部构造出"typename Iterator::IteratorCategory\(\)"，并调用三个参数的\_\_Advance函数。而这，就是基于重载确定的编译期分派技术。请看以下示例：

``` Cpp
// 最终的"多功能函数"
template <typename Iterator>
void Advance(Iterator &iter, int N)
{
    // 根据typename Iterator::IteratorCategory()进行基于重载确定的编译期分派
    __Advance(iter, N, typename Iterator::IteratorCategory());
}


int main()
{
    ForwardIterator<int> forwardIterator(0);
    BidirectionalIterator<int> bidirectionalIterator(0);
    RandomAccessIterator<int> randomAccessIterator(0);

    // 重载确定至__Advance(Iterator &iter, int N, ForwardIteratorTag)版本
    Advance(forwardIterator, 10);

    // 重载确定至__Advance(Iterator &iter, int N, BidirectionalIteratorTag)版本
    Advance(bidirectionalIterator, 10);

    // 重载确定至__Advance(Iterator &iter, int N, RandomAccessIteratorTag)版本
    Advance(randomAccessIterator, 10);

    cout << *forwardIterator << endl;        // 10
    cout << *bidirectionalIterator << endl;  // 10
    cout << *randomAccessIterator << endl;   // 10
}
```

至此，我们就完成了Advance这个"多功能函数"的实现。但最后，我们还有一个重要问题需要解决：指针也是迭代器，那么指针的迭代器类型（当然是随机访问迭代器）怎么获取？

也许不用我说，你就已经知道答案了，解决方案就是"加中间层可解决一切问题"定理。我们可以为"获取迭代器类型"这一操作添加一个中间层，并在此中间层中，对指针类型进行特化。请看以下示例：

``` Cpp
// 迭代器的迭代器类型
template <typename Iterator>
struct IteratorTraits
{
    typedef typename Iterator::IteratorCategory IteratorCategory;
};


// 指针的迭代器类型
template <typename T>
struct IteratorTraits<T *>
{
    typedef RandomAccessIteratorTag IteratorCategory;
};
```

同时，我们还需要将上面的Advance函数中的"简单粗暴的typename Iterator::IteratorCategory"替换为我们刚刚实现的IteratorTraits方法：

``` Cpp
template <typename Iterator>
void Advance(Iterator &iter, int N)
{
    // 根据typename IteratorTraits<Iterator>::IteratorCategory()进行基于重载确定的编译期分派
    __Advance(iter, N, typename IteratorTraits<Iterator>::IteratorCategory());
}
```

至此，Advance函数的实现也就全部完成了。

接下来，我们以一个更为复杂，也更为神奇的功能，继续讨论编译期分派这一技术。

### 5.2 "万能的"打印函数

本节将要实现的功能，其最终使用起来十分简单：

``` Cpp
print(X);  // "X"可以是任何值！
```

没错，这是一个可以打印任何值的函数！通过上一节的铺垫，我们知道：作为实现者，我们不仅可以得到X的值，还能"顺便"得到X的类型。所以，我们就可以在X的类型上大做文章，针对不同的类型实现出不同的打印函数，最后，通过这个print函数进行编译期分派，从而实现出这一神奇的函数。

首先应该实现什么呢？不难发现，X可以是一个"可以直接cout的值"、（支持迭代器的）容器、Pair、Tuple、Stack、Queue等类别。所以，我们首先需要对X的这些不同的类别进行分类，通过创建很多空的Tag类即可实现此功能。请看以下示例：

``` Cpp
// 默认类别
struct __CommonTag {};

// 线性容器类别
struct __SequenceContainerTag {};

// Map容器类别
struct __MapContainerTag {};

// Set容器类别
struct __SetContainerTag {};

// Pair类别
struct __PairTag {};

// Map中的Pair类别
struct __MapPairTag {};

// Tuple类别
struct __TupleTag {};

// Stack类别
struct __StackTag {};

// Queue类别
struct __QueueTag {};
```

然后，通过建立一系列的Traits，我们就可以将X的类型映射到这些Tag上：

``` Cpp
// 如果T不是下面所提及的类型中的任何一种，那么T的类别就是__CommonTag（即："可以直接cout的类型"）
template <typename T>
struct __CategoryTraits
{
    typedef __CommonTag __Category;
};


// array<T, N>的类别是__SequenceContainerTag
template <typename T, size_t N>
struct __CategoryTraits<array<T, N>>
{
    typedef __SequenceContainerTag __Category;
};


// deque<T>的类别是__SequenceContainerTag
template <typename T>
struct __CategoryTraits<deque<T>>
{
    typedef __SequenceContainerTag __Category;
};


// forward_list<T>的类别是__SequenceContainerTag
template <typename T>
struct __CategoryTraits<forward_list<T>>
{
    typedef __SequenceContainerTag __Category;
};


// list<T>的类别是__SequenceContainerTag
template <typename T>
struct __CategoryTraits<list<T>>
{
    typedef __SequenceContainerTag __Category;
};


// vector<T>的类别是__SequenceContainerTag
template <typename T>
struct __CategoryTraits<vector<T>>
{
    typedef __SequenceContainerTag __Category;
};


// map<K, V>的类别是__MapContainerTag
template <typename K, typename V>
struct __CategoryTraits<map<K, V>>
{
    typedef __MapContainerTag __Category;
};


// multimap<K, V>的类别是__MapContainerTag
template <typename K, typename V>
struct __CategoryTraits<multimap<K, V>>
{
    typedef __MapContainerTag __Category;
};


// unordered_map<K, V>的类别是__MapContainerTag
template <typename K, typename V>
struct __CategoryTraits<unordered_map<K, V>>
{
    typedef __MapContainerTag __Category;
};


// unordered_multimap<K, V>的类别是__MapContainerTag
template <typename K, typename V>
struct __CategoryTraits<unordered_multimap<K, V>>
{
    typedef __MapContainerTag __Category;
};


// set<T>的类别是__SetContainerTag
template <typename T>
struct __CategoryTraits<set<T>>
{
    typedef __SetContainerTag __Category;
};


// multiset<T>的类别是__SetContainerTag
template <typename T>
struct __CategoryTraits<multiset<T>>
{
    typedef __SetContainerTag __Category;
};


// unordered_set<T>的类别是__SetContainerTag
template <typename T>
struct __CategoryTraits<unordered_set<T>>
{
    typedef __SetContainerTag __Category;
};


// unordered_multiset<T>的类别是__SetContainerTag
template <typename T>
struct __CategoryTraits<unordered_multiset<T>>
{
    typedef __SetContainerTag __Category;
};


// pair<T1, T2>的类别是__PairTag
template <typename T1, typename T2>
struct __CategoryTraits<pair<T1, T2>>
{
    typedef __PairTag __Category;
};


// tuple<Types...>的类别是__TupleTag
template <typename... Types>
struct __CategoryTraits<tuple<Types...>>
{
    typedef __TupleTag __Category;
};


// stack<T>的类别是__StackTag
template <typename T>
struct __CategoryTraits<stack<T>>
{
    typedef __StackTag __Category;
};


// queue<T>的类别是__QueueTag
template <typename T>
struct __CategoryTraits<queue<T>>
{
    typedef __QueueTag __Category;
};


// priority_queue<T>的类别是__StackTag
template <typename T>
struct __CategoryTraits<priority_queue<T>>
{
    typedef __StackTag __Category;
};
```

接下来需要解决的问题是：我们希望面对类似于一维数组这样的"较小的值"和类似于高维数组这样的"较大的值"时，能够采用或紧凑，或分散的不同的打印格式。具体什么是"较小的值"呢？这里，我们认为：如果X本身的类别就是\_\_CommonTag，或X中的值（前提是X本身的类别不是\_\_CommonTag）的类别是\_\_CommonTag，则认为X是一个"较小的值"，采取紧凑的打印格式，否则，就认为X是一个"较大的值"，采取分散的打印格式。

那么，我们就又引出了一个问题：对于某些类型，如Pair和Tuple，其中的各个元素的类型是不一样的，即：各个元素的类别也是不一样的；同时，很显然，当我们面对多个类别时，只要其中有一个类别不是\_\_CommonTag，那么我们就应当认为这些类别的"最强大类别"不是\_\_CommonTag。因此，我们首先需要实现一个获取"最强大类别"的Traits。请看以下示例：

``` Cpp
// 原型
// 通过typename __CategoryPromotionTraits<Tags...>::__Category获取"最强大类别"
template <typename... Tags>
struct __CategoryPromotionTraits;


// 如果有两个任意的类别，则随便选一个类别作为"更强大类别"即可...
template <typename Tag1, typename Tag2>
struct __CategoryPromotionTraits<Tag1, Tag2>
{
    typedef Tag1 __Category;
};


// ...但是，如果右类别是__CommonTag，则"更强大类别"需要"敬而远之"...
template <typename Tag1>
struct __CategoryPromotionTraits<Tag1, __CommonTag>
{
    typedef Tag1 __Category;
};


// ...同理，如果左类别是__CommonTag，则"更强大类别"也需要"敬而远之"...
template <typename Tag2>
struct __CategoryPromotionTraits<__CommonTag, Tag2>
{
    typedef Tag2 __Category;
};


// ...只有当"你我都是普通人"时，"更强大类别"才是__CommonTag
template <>
struct __CategoryPromotionTraits<__CommonTag, __CommonTag>
{
    typedef __CommonTag __Category;
};


// 面对不止两个类别时，"最强大类别"应该是Tag1与Tags...的"最强大类别"之间的"更强大类别"
template <typename Tag1, typename... Tags>
struct __CategoryPromotionTraits<Tag1, Tags...>
{
    typedef typename __CategoryPromotionTraits<
        Tag1,
        typename __CategoryPromotionTraits<Tags...>::__Category
    >::__Category __Category;
};
```

接下来，我们就来实现能够获取X中的元素的类别（即X的"子类别"）的Traits：

``` Cpp
// 原型
// 通过typename __SubCategoryTraits<X的类别, X的类型>::__Category获取X中的元素的类别
template <typename Tag, typename T>
struct __SubCategoryTraits;


// __CommonTag没有子类别
template <typename T>
struct __SubCategoryTraits<__CommonTag, T>
{
    typedef void __Category;
};


// __SequenceContainerTag的子类别就是T::value_type的类别
template <typename T>
struct __SubCategoryTraits<__SequenceContainerTag, T>
{
    typedef typename __CategoryTraits<typename T::value_type>::__Category __Category;
};


// __MapContainerTag的子类别一定是__MapPairTag
template <typename T>
struct __SubCategoryTraits<__MapContainerTag, T>
{
    typedef __MapPairTag __Category;
};


// __SetContainerTag的子类别就是T::value_type的类别
template <typename T>
struct __SubCategoryTraits<__SetContainerTag, T>
{
    typedef typename __CategoryTraits<typename T::value_type>::__Category __Category;
};


// __MapPairTag的子类别是T::first_type的类别与T::second_type的类别之间的"更强大类别"
template <typename T>
struct __SubCategoryTraits<__MapPairTag, T>
{
    typedef typename __CategoryPromotionTraits<
        typename __CategoryTraits<typename T::first_type>::__Category,
        typename __CategoryTraits<typename T::second_type>::__Category
    >::__Category __Category;
};


// 和__MapPairTag一样
// __PairTag的子类别也是T::first_type的类别与T::second_type的类别之间的"更强大类别"
template <typename T>
struct __SubCategoryTraits<__PairTag, T>
{
    typedef typename __CategoryPromotionTraits<
        typename __CategoryTraits<typename T::first_type>::__Category,
        typename __CategoryTraits<typename T::second_type>::__Category
    >::__Category __Category;
};


// __TupleTag的子类别是各个Types的类别之中的"最强大类别"
template <typename... Types>
struct __SubCategoryTraits<__TupleTag, tuple<Types...>>
{
    typedef typename __CategoryPromotionTraits<
        typename __CategoryTraits<Types>::__Category...
    >::__Category __Category;
};


// __StackTag的子类别就是T::value_type的类别
template <typename T>
struct __SubCategoryTraits<__StackTag, T>
{
    typedef typename __CategoryTraits<typename T::value_type>::__Category __Category;
};


// __QueueTag的子类别就是T::value_type的类别
template <typename T>
struct __SubCategoryTraits<__QueueTag, T>
{
    typedef typename __CategoryTraits<typename T::value_type>::__Category __Category;
};
```

有了\_\_CategoryTraits和\_\_SubCategoryTraits这两个工具，我们的准备工作也就基本上完成了。接下来是最后的一些简单的准备工作：

1. 我们需要定义一些控制打印样式的字符或字符串常量。请看以下示例：

``` Cpp
// 缩进空白字符
const char   __SPACE                    = ' ';

// 单次缩进长度
const int    __INDENTATION_LEN          = 4;

// 元素之间的分隔符
const string __VALUE_SPLICE             = ", ";

// 行末分隔符
const char   __LINE_END                 = ',';

// 线性容器类别的左右定界符
const char   __SEQUENCE_CONTAINER_BEGIN = '[';
const char   __SEQUENCE_CONTAINER_END   = ']';

// Map容器类别的左右定界符
const char   __MAP_CONTAINER_BEGIN      = '{';
const char   __MAP_CONTAINER_END        = '}';

// Set容器类别的左右定界符
const char   __SET_CONTAINER_BEGIN      = '{';
const char   __SET_CONTAINER_END        = '}';

// Pair类别的左右定界符
const char   __PAIR_BEGIN               = '(';
const char   __PAIR_END                 = ')';

// Map中的Pair类别的左右定界符
const char   __MAP_PAIR_BEGIN           = '(';
const char   __MAP_PAIR_END             = ')';

// Map中的Pair类别的键值对分隔符
const string __MAP_PAIR_SPLICE          = ": ";

// Map中的Pair类别的键值对行末分隔符
const char   __MAP_PAIR_LINE_END        = ':';

// Tuple容器类别的左右定界符
const char   __TUPLE_BEGIN              = '(';
const char   __TUPLE_END                = ')';

// Stack容器类别的左右定界符
const char   __STACK_BEGIN              = '[';
const char   __STACK_END                = ']';

// Queue容器类别的左右定界符
const char   __QUEUE_BEGIN              = '[';
const char   __QUEUE_END                = ']';
```

2. 对于Stack\<int\>，如果我们将1，2，3依次入栈，则取出时只能按3，2，1的顺序出栈，这将造成很奇怪的打印效果。所以我们需要一个简单的函数，用于将Stack反序。请看以下示例：

``` Cpp
template <typename T>
T __reverseStack(T oriStack)  // 必须使用值传递
{
    T reverseStack;

    while (!oriStack.empty())
    {
        reverseStack.push(oriStack.top());
        oriStack.pop();
    }

    return reverseStack;
}
```

终于可以开始着手实现最重要的print函数了！这里我们通过实现很多个\_\_PrintData类的特化来实现针对不同类别及其子类别组合的编译期分派。那么，\_\_PrintData类需要哪些模板参数呢？X的类别、X的子类别、X的类型，显然都是需要的，此外，我们还需要在模板参数中维护一个整数，用于使每个模板都能够知道"我在第几层？"，以实现打印时的缩进控制。请看以下示例：

``` Cpp
// 原型
// 通过__PrintData<SelfTag, SubTag, T, N>::__Print(val)打印val
template <typename SelfTag, typename SubTag, typename T, int N>
struct __PrintData;


// 打印__CommonTag类别的值（__CommonTag的子类别一定是void），采用紧凑的打印格式
template <typename T, int N>
struct __PrintData<__CommonTag, void, T, N>
{
    static void __Print(const T &val);
};


// 打印__SequenceContainerTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
struct __PrintData<__SequenceContainerTag, SubTag, T, N>
{
    static void __Print(const T &val);
};


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
struct __PrintData<__SequenceContainerTag, __CommonTag, T, N>
{
    static void __Print(const T &val);
};


// 打印__MapContainerTag类别 + __MapPairTag子类别组合的值，采用宽松的打印格式
template <typename T, int N>
struct __PrintData<__MapContainerTag, __MapPairTag, T, N>
{
    static void __Print(const T &val);
};


// 打印__SetContainerTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
struct __PrintData<__SetContainerTag, SubTag, T, N>
{
    static void __Print(const T &val);
};


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
struct __PrintData<__SetContainerTag, __CommonTag, T, N>
{
    static void __Print(const T &val);
};


// 打印__PairTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
struct __PrintData<__PairTag, SubTag, T, N>
{
    static void __Print(const T &val);
};


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
struct __PrintData<__PairTag, __CommonTag, T, N>
{
    static void __Print(const T &val);
};


// 打印__MapPairTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
struct __PrintData<__MapPairTag, SubTag, T, N>
{
    static void __Print(const T &val);
};


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
struct __PrintData<__MapPairTag, __CommonTag, T, N>
{
    static void __Print(const T &val);
};


// 打印__TupleTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
struct __PrintData<__TupleTag, SubTag, T, N>
{
    static void __Print(const T &val);
};


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
struct __PrintData<__TupleTag, __CommonTag, T, N>
{
    static void __Print(const T &val);
};


// 打印__StackTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
struct __PrintData<__StackTag, SubTag, T, N>
{
    static void __Print(const T &val);
};


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
struct __PrintData<__StackTag, __CommonTag, T, N>
{
    static void __Print(const T &val);
};


// 打印__QueueTag + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
struct __PrintData<__QueueTag, SubTag, T, N>
{
    static void __Print(T val);
};


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
struct __PrintData<__QueueTag, __CommonTag, T, N>
{
    static void __Print(T val);
};
```

以下是除了Tuple以外的（Tuple的打印函数的实现我们将在稍后讨论）各个打印函数的实现：

``` Cpp
// 打印__CommonTag类别的值（__CommonTag的子类别一定是void），采用紧凑的打印格式
template <typename T, int N>
void __PrintData<__CommonTag, void, T, N>::__Print(const T &val)
{
    // 直接打印缩进与val
    cout << string(N * __INDENTATION_LEN, __SPACE) << val;
}


// 打印__SequenceContainerTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
void __PrintData<__SequenceContainerTag, SubTag, T, N>::__Print(const T &val)
{
    // 打印缩进与线性容器类别的左定界符，然后换行
    cout << string(N * __INDENTATION_LEN, __SPACE) << __SEQUENCE_CONTAINER_BEGIN << endl;

    for (auto &subVal: val)
    {
        // 缩进层数+1，使用线性容器中的元素的__PrintData版本，依次打印线性容器中的每个元素
        __PrintData<
            SubTag,
            typename __SubCategoryTraits<SubTag, typename T::value_type>::__Category,
            typename T::value_type,
            N + 1
        >::__Print(subVal);

        // 打印行末分隔符后换行
        cout << __LINE_END << endl;
    }

    // 打印缩进与线性容器类别的右定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __SEQUENCE_CONTAINER_END;
}


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
void __PrintData<__SequenceContainerTag, __CommonTag, T, N>::__Print(const T &val)
{
    // 打印缩进与线性容器类别的左定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __SEQUENCE_CONTAINER_BEGIN;

    // 不换行，依次打印线性容器中的每个元素，以及元素之间的分隔符
    if (!val.empty())
    {
        cout << val.front();

        for (auto iter = next(val.begin()); iter != val.end(); iter++)
        {
            cout << __VALUE_SPLICE << *iter;
        }
    }

    // 打印线性容器类别的右定界符
    cout << __SEQUENCE_CONTAINER_END;
}


// 打印__MapContainerTag类别 + __MapPairTag子类别组合的值，采用宽松的打印格式
template <typename T, int N>
void __PrintData<__MapContainerTag, __MapPairTag, T, N>::__Print(const T &val)
{
    // 打印缩进与Map容器类别的左定界符，然后换行
    cout << string(N * __INDENTATION_LEN, __SPACE) << __MAP_CONTAINER_BEGIN << endl;

    for (auto &subVal: val)
    {
        // 缩进层数+1，直接使用__MapPairTag类别以及Map容器的子类别生成__PrintData，用于打印Map中的Pair
        __PrintData<
            __MapPairTag,
            typename __SubCategoryTraits<__MapPairTag, typename T::value_type>::__Category,
            typename T::value_type,
            N + 1
        >::__Print(subVal);

        // 打印行末分隔符后换行
        cout << __LINE_END << endl;
    }

    // 打印缩进与Map容器类别的右定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __MAP_CONTAINER_END;
}


// 打印__SetContainerTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
void __PrintData<__SetContainerTag, SubTag, T, N>::__Print(const T &val)
{
    // 打印缩进与Set容器类别的左定界符，然后换行
    cout << string(N * __INDENTATION_LEN, __SPACE) << __SET_CONTAINER_BEGIN << endl;

    for (auto &subVal: val)
    {
        // 缩进层数+1，使用Set容器中的元素的__PrintData版本，依次打印Set容器中的每个元素
        __PrintData<
            SubTag,
            typename __SubCategoryTraits<SubTag, typename T::value_type>::__Category,
            typename T::value_type,
            N + 1
        >::__Print(subVal);

        // 打印行末分隔符后换行
        cout << __LINE_END << endl;
    }

    // 打印缩进与Set容器类别的右定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __SET_CONTAINER_END;
}


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
void __PrintData<__SetContainerTag, __CommonTag, T, N>::__Print(const T &val)
{
    // 打印缩进与Set容器类别的左定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __SET_CONTAINER_BEGIN;

    // 不换行，依次打印Set容器中的每个元素，以及元素之间的分隔符
    if (!val.empty())
    {
        cout << *val.begin();

        for (auto iter = next(val.begin()); iter != val.end(); iter++)
        {
            cout << __VALUE_SPLICE << *iter;
        }
    }

    // 打印Set容器类别的右定界符
    cout << __SET_CONTAINER_END;
}


// 打印__PairTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
void __PrintData<__PairTag, SubTag, T, N>::__Print(const T &val)
{
    // 打印缩进与Pair类别的左定界符，然后换行
    cout << string(N * __INDENTATION_LEN, __SPACE) << __PAIR_BEGIN << endl;

    // 缩进层数+1，使用val.first的__PrintData版本打印val.first
    __PrintData<

        typename __CategoryTraits<typename T::first_type>::__Category,

        typename __SubCategoryTraits<
            typename __CategoryTraits<typename T::first_type>::__Category,
            typename T::first_type
        >::__Category,

        typename T::first_type,

        N + 1

    >::__Print(val.first);

    // 打印行末分隔符后换行
    cout << __LINE_END << endl;

    // 缩进层数+1，使用val.second的__PrintData版本打印val.second
    __PrintData<

        typename __CategoryTraits<typename T::second_type>::__Category,

        typename __SubCategoryTraits<
            typename __CategoryTraits<typename T::second_type>::__Category,
            typename T::second_type
        >::__Category,

        typename T::second_type,

        N + 1

    >::__Print(val.second);

    // 打印行末分隔符后换行，再打印缩进与Pair类别的右定界符
    cout << __LINE_END << endl << string(N * __INDENTATION_LEN, __SPACE) << __PAIR_END;
}


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
void __PrintData<__PairTag, __CommonTag, T, N>::__Print(const T &val)
{
    // 直接依次打印缩进、Pair类别的左定界符、val.first、元素之间的分隔符、
    // val.second以及Pair类别的右定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __PAIR_BEGIN <<
        val.first << __VALUE_SPLICE << val.second << __PAIR_END;
}


// 打印__MapPairTag类别 + 任意子类别组合的值，采用宽松的打印格式...
// 此版本的实现与__PrintData<__PairTag, SubTag, T, N>版本高度相似
// 唯一的区别在于定界符的选取不同
template <typename SubTag, typename T, int N>
void __PrintData<__MapPairTag, SubTag, T, N>::__Print(const T &val)
{
    // 打印缩进与Map中的Pair类别的左定界符，然后换行
    cout << string(N * __INDENTATION_LEN, __SPACE) << __MAP_PAIR_BEGIN << endl;

    // 缩进层数+1，使用val.first的__PrintData版本打印val.first
    __PrintData<

        typename __CategoryTraits<typename T::first_type>::__Category,

        typename __SubCategoryTraits<
            typename __CategoryTraits<typename T::first_type>::__Category,
            typename T::first_type
        >::__Category,

        typename T::first_type,

        N + 1

    >::__Print(val.first);

    // 打印Map中的Pair类别的键值对行末分隔符后换行
    cout << __MAP_PAIR_LINE_END << endl;

    // 缩进层数+1，使用val.second的__PrintData版本打印val.second
    __PrintData<

        typename __CategoryTraits<typename T::second_type>::__Category,

        typename __SubCategoryTraits<
            typename __CategoryTraits<typename T::second_type>::__Category,
            typename T::second_type
        >::__Category,

        typename T::second_type,

        N + 1

    >::__Print(val.second);

    // 打印行末分隔符后换行，再打印缩进与Map中的Pair类别的右定界符
    cout << __LINE_END << endl << string(N * __INDENTATION_LEN, __SPACE) << __MAP_PAIR_END;
}


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
// 此版本的实现与__PrintData<__PairTag, __CommonTag, T, N>版本高度相似
// 唯一的区别在于定界符的选取不同
template <typename T, int N>
void __PrintData<__MapPairTag, __CommonTag, T, N>::__Print(const T &val)
{
    // 直接依次打印缩进、Map中的Pair类别的左定界符、val.first、
    // Map中的Pair类别的键值对分隔符、val.second以及Map中的Pair类别的右定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __MAP_PAIR_BEGIN <<
        val.first << __MAP_PAIR_SPLICE << val.second << __MAP_PAIR_END;
}


// 打印__StackTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
void __PrintData<__StackTag, SubTag, T, N>::__Print(const T &val)
{
    // 得到一个反序的Stack
    T reverseVal = __reverseStack(val);

    // 打印缩进与Stack容器类别的左定界符，然后换行
    cout << string(N * __INDENTATION_LEN, __SPACE) << __STACK_BEGIN << endl;

    while (!reverseVal.empty())
    {
        // 缩进层数+1，使用Stack容器中的元素的__PrintData版本，依次打印Stack容器中的每个元素
        __PrintData<
            SubTag,
            typename __SubCategoryTraits<SubTag, typename T::value_type>::__Category,
            typename T::value_type,
            N + 1
        >::__Print(reverseVal.top());

        // 打印行末分隔符后换行
        cout << __LINE_END << endl;

        // 出栈
        reverseVal.pop();
    }

    // 打印缩进与Stack容器类别的右定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __STACK_END;
}


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
void __PrintData<__StackTag, __CommonTag, T, N>::__Print(const T &val)
{
    // 得到一个反序的Stack
    T reverseVal = __reverseStack(val);

    // 打印缩进与Stack容器类别的左定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __STACK_BEGIN;

    // 不换行，依次打印Stack容器中的每个元素，以及元素之间的分隔符
    if (!reverseVal.empty())
    {
        cout << reverseVal.top();
        reverseVal.pop();

        while (!reverseVal.empty())
        {
            cout << __VALUE_SPLICE << reverseVal.top();
            reverseVal.pop();
        }
    }

    // 打印Stack容器类别的右定界符
    cout << __STACK_END;
}


// 打印__QueueTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
void __PrintData<__QueueTag, SubTag, T, N>::__Print(T val)
{
    // 打印缩进与Queue容器类别的左定界符，然后换行
    cout << string(N * __INDENTATION_LEN, __SPACE) << __QUEUE_BEGIN << endl;

    while (!val.empty())
    {
        // 缩进层数+1，使用Queue容器中的元素的__PrintData版本，依次打印Queue容器中的每个元素
        __PrintData<
            SubTag,
            typename __SubCategoryTraits<SubTag, typename T::value_type>::__Category,
            typename T::value_type,
            N + 1
        >::__Print(val.front());

        // 打印行末分隔符后换行
        cout << __LINE_END << endl;

        // 出队列
        val.pop();
    }

    // 打印缩进与Queue容器类别的右定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __QUEUE_END;
}


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
void __PrintData<__QueueTag, __CommonTag, T, N>::__Print(T val)
{
    // 打印缩进与Queue容器类别的左定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __QUEUE_BEGIN;

    // 不换行，依次打印Queue容器中的每个元素，以及元素之间的分隔符
    if (!val.empty())
    {
        cout << val.front();
        val.pop();

        while (!val.empty())
        {
            cout << __VALUE_SPLICE << val.front();
            val.pop();
        }
    }

    // 打印Queue容器类别的右定界符
    cout << __QUEUE_END;
}
```

怎么打印Tuple？虽然Tuple不能通过真正的for循环进行遍历，但我们可以使用编译期的"for循环"对Tuple进行"遍历"。请看以下示例：

``` Cpp
// 以紧凑（单行）的打印格式打印Tuple
// 从"索引值"0，向"最大索引值"TopIdx进行"迭代"
template <typename T, int Idx, int TopIdx>
struct __PrintTupleOneLine
{
    static void __Print(const T &val);
};


// 当Idx == TopIdx时，"迭代"结束
template <typename T, int TopIdx>
struct __PrintTupleOneLine<T, TopIdx, TopIdx>
{
    static void __Print(const T &val) {}
};


// 以宽松（多行）的打印格式打印Tuple
// 从"索引值"0，向"最大索引值"TopIdx进行"迭代"
template <typename T, int N, int Idx, int TopIdx>
struct __PrintTupleMultiLine
{
    static void __Print(const T &val);
};


// 当Idx == TopIdx时，"迭代"结束
template <typename T, int N, int TopIdx>
struct __PrintTupleMultiLine<T, N, TopIdx, TopIdx>
{
    static void __Print(const T &val) {}
};


template <typename T, int Idx, int TopIdx>
void __PrintTupleOneLine<T, Idx, TopIdx>::__Print(const T &val)
{
    // 打印"val[Idx]"
    cout << get<Idx>(val);

    // 如果"val[Idx]"不是Tuple的最后一个元素，则打印元素之间的分隔符
    if (Idx + 1 < TopIdx)
    {
        cout << __VALUE_SPLICE;
    }

    // 继续打印"val[Idx + 1]"...
    __PrintTupleOneLine<T, Idx + 1, TopIdx>::__Print(val);
}


template <typename T, int N, int Idx, int TopIdx>
void __PrintTupleMultiLine<T, N, Idx, TopIdx>::__Print(const T &val)
{
    // 缩进层数+1，使用"val[Idx]"的__PrintData版本打印"val[Idx]"
    __PrintData<

        typename __CategoryTraits<typename tuple_element<Idx, T>::type>::__Category,

        typename __SubCategoryTraits<
            typename __CategoryTraits<typename tuple_element<Idx, T>::type>::__Category,
            typename tuple_element<Idx, T>::type
        >::__Category,

        typename tuple_element<Idx, T>::type,

        N + 1

    >::__Print(get<Idx>(val));

    // 打印行末分隔符后换行
    cout << __LINE_END << endl;

    // 继续打印"val[Idx + 1]"...
    __PrintTupleMultiLine<T, N, Idx + 1, TopIdx>::__Print(val);
}
```

此时，我们就可以提供用于打印Tuple的\_\_PrintData的定义了：

``` Cpp
// 打印__TupleTag类别 + 任意子类别组合的值，采用宽松的打印格式...
template <typename SubTag, typename T, int N>
void __PrintData<__TupleTag, SubTag, T, N>::__Print(const T &val)
{
    // 打印缩进与Tuple容器类别的左定界符，然后换行
    cout << string(N * __INDENTATION_LEN, __SPACE) << __TUPLE_BEGIN << endl;

    // 调用多行版本的Tuple打印函数，打印val
    __PrintTupleMultiLine<T, N, 0, tuple_size<T>::value>::__Print(val);

    // 打印Tuple容器类别的右定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __TUPLE_END;
}


// ...但是，如果子类别是__CommonTag，则采用紧凑的打印格式
template <typename T, int N>
void __PrintData<__TupleTag, __CommonTag, T, N>::__Print(const T &val)
{
    // 打印缩进与Tuple容器类别的左定界符
    cout << string(N * __INDENTATION_LEN, __SPACE) << __TUPLE_BEGIN;

    // 调用单行版本的Tuple打印函数，打印val
    __PrintTupleOneLine<T, 0, tuple_size<T>::value>::__Print(val);

    // 打印Tuple容器类别的右定界符
    cout << __TUPLE_END;
}
```

至此，我们已经实现了print函数所需要的一切底层组件。现在我们需要做的，就是汇聚所有的这些底层组件，最终实现print函数。请看以下示例：

``` Cpp
template <typename T>
void print(const T &val)
{
    __PrintData<

        // T的类别
        typename __CategoryTraits<T>::__Category,

        // T的子类别
        typename __SubCategoryTraits<
            typename __CategoryTraits<T>::__Category,
            T
        >::__Category,

        // val的类型
        T,

        // 缩进层数从0开始
        0

    >::__Print(val);

    cout << endl;
}
```

让我们立即来试试这个print函数的效果：

``` Cpp
int main()
{
    // 普通值
    int sampleInt = 123;
    double *samplePtr = nullptr;
    string sampleStr = "abc";

    print(sampleInt);  // 123
    print(samplePtr);  // 0
    print(sampleStr);  // abc


    // 线性容器
    array<int, 3> sampleArray {1, 2, 3};
    vector<string> sampleVector {"abc", "def", "ghi"};
    list<deque<forward_list<string>>> sampleComplexContainer {{{"abc", "def"}, {"ghi", "jkl"}}, {{"mno", "pqr"}, {"stu", "vwx"}}};

    print(sampleArray);   // [1, 2, 3]
    print(sampleVector);  // [abc, def, ghi]

    /*
    [
        [
            [abc, def],
            [ghi, jkl],
        ],
        [
            [mno, pqr],
            [stu, vwx],
        ],
    ]
    */
    print(sampleComplexContainer);


    // Map容器
    map<int, string> sampleMap {{1, "abc"}, {2, "def"}, {3, "ghi"}};
    multimap<int, vector<string>> sampleComplexMap {{1, {"abc", "def"}}, {2, {"ghi", "jkl"}}, {3, {"mno", "pqu"}}};

    /*
    {
        (1: abc),
        (2: def),
        (3: ghi),
    }
    */
    print(sampleMap);

    /*
    {
        (
            1:
            [abc, def],
        ),
        (
            2:
            [ghi, jkl],
        ),
        (
            3:
            [mno, pqu],
        ),
    }
    */
    print(sampleComplexMap);


    // Set容器
    set<int> sampleSet {1, 2, 3};
    multiset<vector<bool>> sampleComplexSet {{true, false}, {false, true}, {true, false, false, true}};

    print(sampleSet);  // {1, 2, 3}

    /*
    {
        [0, 1],
        [1, 0],
        [1, 0, 0, 1],
    }
    */
    print(sampleComplexSet);


    // Pair
    pair<int, string> samplePair {1, "abc"};
    pair<int, vector<string>> sampleComplexPair {1, {"abc", "def", "ghi"}};

    print(samplePair);  // (1, abc)

    /*
    (
        1,
        [abc, def, ghi],
    )
    */
    print(sampleComplexPair);


    // Tuple容器
    tuple<int, double, char, string> sampleTuple {1, 2., 'a', "abc"};
    tuple<int, double, char, string, vector<string>> sampleComplexTuple {1, 2., 'a', "abc", {"abc", "def", "ghi"}};

    print(sampleTuple);  // (1, 2, a, abc)

    /*
    (
        1,
        2,
        a,
        abc,
        [abc, def, ghi],
    )
    */
    print(sampleComplexTuple);


    // Stack容器
    stack<int> sampleStack;

    sampleStack.push(1);
    sampleStack.push(2);
    sampleStack.push(3);

    stack<vector<string>> sampleComplexStack;

    sampleComplexStack.push({"abc", "def"});
    sampleComplexStack.push({"ghi", "jkl"});
    sampleComplexStack.push({"mno", "pqr"});

    /*
    栈底 --------> 栈顶
         [1, 2, 3]
    */
    print(sampleStack);

    /*
                     栈底
    [                 |
        [abc, def],   |
        [ghi, jkl],   |
        [mno, pqr],   |
    ]                 v
                     栈顶
    */
    print(sampleComplexStack);


    // Queue容器
    queue<int> sampleQueue;

    sampleQueue.push(1);
    sampleQueue.push(2);
    sampleQueue.push(3);

    priority_queue<vector<string>> sampleComplexPriorityQueue;

    sampleComplexPriorityQueue.push({"abc", "def"});
    sampleComplexPriorityQueue.push({"ghi", "jkl"});
    sampleComplexPriorityQueue.push({"mno", "pqr"});

    /*
    队列首 <-------- 队列尾
           [1, 2, 3]
    */
    print(sampleQueue);

    /*
                     队列首
    [                  ^
        [mno, pqr],    |
        [ghi, jkl],    |
        [abc, def],    |
    ]                  |
                     队列尾
    */
    print(sampleComplexPriorityQueue);
}
```

至此，print函数的实现也就全部完成了。

### 5.3 本章后记

本章，我们首先通过一个简单的STL advance函数，讨论了编译期分派技术。这一函数的实现过程能够带给我们两点思考：

1. 为什么我们能在用户无感知的情况下实现分派呢？不难发现：你，作为一个函数的实现者，有一样东西是用户所不具有的，那就是类型。当用户使用一个值来调用函数时，你不仅能得到这个值，还能"顺便"得到这个值的类型，而正是因为有了这多出来的类型，我们就能在类型上大做文章，实现出很多"神奇"的接口
2. 事实上，并没有真正的"多功能函数"。但我们可以通过"添加中间层可解决一切问题"这一"经典定理"，使得单一的接口函数，根据传入的实参类型的不同，"多功能的"调用多个底层的实现函数，从而达到"多功能函数"的效果

紧接着，我们实现了一个代码更为复杂的print函数。观其输出结果，不禁让我们感慨：一个小小的T，在经过我们的"大做文章"之后，竟能够表现出如此丰富的多样性！这，就是编译期分派的强大威力所在！

## 6 "突破极限"的容器——Tuple

Tuple是一种非常特殊且高级的数据结构，其能够容纳和取用数量、类型都不定的一组值，你也可以将Tuple理解为某种"匿名结构体"。乍看之下，"数量、类型都不定"和模板中"什么都是已经确定的编译期常量"从语法上就是完全相悖的，和容器的"所有元素的类型必须相同"的原则也是完全相悖的，似乎，Tuple是一种"突破极限"的容器。可事实真的是如此吗？

### 6.1 可递归Pair

首先，请看下面这段"平淡无奇"的代码：

``` Cpp
template <typename T1, typename T2>
struct __RecursionPair
{
public:

    // 数据成员
    T1 __first;
    T2 __second;


    // 构造函数
    __RecursionPair();
    __RecursionPair(const T1 &first, const T2 &second);
};


template <typename T1, typename T2>
__RecursionPair<T1, T2>::__RecursionPair() = default;


template <typename T1, typename T2>
__RecursionPair<T1, T2>::__RecursionPair(const T1 &first, const T2 &second):
    __first(first), __second(second) {}


// 针对只有一个值的Pair的特化
template <typename T1>
struct __RecursionPair<T1, void>
{
public:

    // 数据成员
    T1 __first;


    // 构造函数
    __RecursionPair();
    explicit __RecursionPair(const T1 &first);
};


template <typename T1>
__RecursionPair<T1, void>::__RecursionPair() = default;


template <typename T1>
__RecursionPair<T1, void>::__RecursionPair(const T1 &first): __first(first) {}


int main()
{
    __RecursionPair<int, double> _(1, 2.);
    __RecursionPair<int, void> __(1);
}
```

"这不就是STL的Pair吗？"，你一定会有这样的疑问。没错，这确实就是STL的Pair，但请你继续看：

``` Cpp
__RecursionPair<int, __RecursionPair<double, __RecursionPair<char, string>>> multiPair;  // 还有这种操作？？？
```

没错！就是有这样的操作。此时，也许你已经意识到了，只要"无限堆叠"这样的Pair，理论上就能实现一个任意数量+任意类型的容器了。我们称这样的Pair为"可递归Pair"。

故事结束了吗？不，这才只是个开始。我们可以立即发现，这种"无限堆叠"产生的"千层饼"，是一个非常反人类的东西，不仅没有"索引值"，甚至为了取得第10个值，竟然需要连着写9遍".second"！这也太反人类了！请不要着急，接着往下看。

### 6.2 为可递归Pair的类型添加"索引值"

接下来，我们需要解决似乎很棘手的一个问题：如何根据"索引值"获取可递归Pair的某个位置的类型呢？要知道，可递归Pair里面可是根本没有"索引值"这一概念啊。

让我们先试着迈出第一步：获取\_\_RecursionPair\<T1, void\>的T1。请看以下示例：

``` Cpp
// 原型
// 通过typename __RecursionPairType<T, N>::__ValueType获取"T[N]"的类型
template <int N, typename T>
struct __RecursionPairType;


// 获取__RecursionPair<T1, void>的第0个类型（相当于"T[0]"的类型）
template <typename T1>
struct __RecursionPairType<0, __RecursionPair<T1, void>>
{
    // __RecursionPair<T1, void>的第0个类型显然就是T1
    typedef T1 __ValueType;
};
```

似乎很顺利对不对？让我们继续：获取\_\_RecursionPair\<T1, T2\>的T1和T2。请看以下示例：

``` Cpp
// 获取__RecursionPair<T1, T2>的第0个类型（相当于"T[0]"的类型）
template <typename T1, typename T2>
struct __RecursionPairType<0, __RecursionPair<T1, T2>>
{
    // __RecursionPair<T1, T2>的第0个类型显然就是T1
    typedef T1 __ValueType;
};


// 获取__RecursionPair<T1, T2>的第1个类型（相当于"T[1]"的类型）
template <typename T1, typename T2>
struct __RecursionPairType<1, __RecursionPair<T1, T2>>
{
    // __RecursionPair<T1, T2>的第1个类型显然就是T2
    typedef T2 __ValueType;
};
```

接下来，我们就要面对真正的难题了，它就是：

``` Cpp
__RecursionPair<T1, __RecursionPair<T2, T3>>
```

仔细分析这一类型不难发现：T1和T2一定不会继续是一个\_\_RecursionPair类型（因为我们人为地"默认"了可递归Pair只有second可以进行递归，实际上first也可以进行递归，但是这样的代码看上去比较"别扭"）。所以，我们立即可以给出以下实现：

``` Cpp
// 获取__RecursionPair<T1, __RecursionPair<T2, T3>>的第0个类型（相当于"T[0]"的类型）
template <typename T1, typename T2, typename T3>
struct __RecursionPairType<0, __RecursionPair<T1, __RecursionPair<T2, T3>>>
{
    // 因为T1一定不会继续是一个__RecursionPair类型
    // 所以__RecursionPair<T1, __RecursionPair<T2, T3>>的第0个类型应该就是T1
    typedef T1 __ValueType;
};


// 获取__RecursionPair<T1, __RecursionPair<T2, T3>>的第1个类型（相当于"T[1]"的类型）
template <typename T1, typename T2, typename T3>
struct __RecursionPairType<1, __RecursionPair<T1, __RecursionPair<T2, T3>>>
{
    // 因为T2一定不会继续是一个__RecursionPair类型
    // 所以__RecursionPair<T1, __RecursionPair<T2, T3>>的第1个类型应该就是T2
    typedef T2 __ValueType;
};
```

那么，如果N大于1，要怎么办呢？此时，虽然我们自己已经无能为力（因为我们并没有能力"拆分"T3），但是我们可以"寄希望于"递归。请看以下示例：

``` Cpp
// 获取__RecursionPair<T1, __RecursionPair<T2, T3>>的第N（N > 1）个类型（相当于"T[N]"的类型）
template <int N, typename T1, typename T2, typename T3>
struct __RecursionPairType<N, __RecursionPair<T1, __RecursionPair<T2, T3>>>
{
    // 如果N大于1，那么"T[N]"的类型应该是__RecursionPair<T2, T3>的第N - 1个类型
    typedef typename __RecursionPairType<N - 1, __RecursionPair<T2, T3>>::__ValueType __ValueType;
};
```

至此，我们就完整实现了根据"索引值"获取可递归Pair的某个位置的类型这一工具。让我们来看看效果：

``` Cpp
int main()
{
    typedef __RecursionPair<int, __RecursionPair<double, __RecursionPair<char, string>>> Type;
    cout << typeid(__RecursionPairType<3, Type>::__ValueType).name();  // string
}
```

可以看出，输出结果完全符合我们的预期。

看到这里，也许你会觉得上述实现中的第4个和第5个特化（即\_\_RecursionPairType\<0, \_\_RecursionPair\<T1, \_\_RecursionPair\<T2, T3\>\>\>和\_\_RecursionPairType\<1, \_\_RecursionPair\<T1, \_\_RecursionPair\<T2, T3\>\>\>版本）似乎是多余的？你可以去掉这些特化，然后编译试试看。

### 6.3 为可递归Pair的值添加"索引值"

有了上文中\_\_RecursionPairType的铺垫，根据"索引值"获取可递归Pair的某个位置的值这一功能似乎也可以"依葫芦画瓢"进行实现了。同样，让我们先迈出第一步：

``` Cpp
// 原型
// 通过__RecursionPairValue<N, T>::__Get(pairObj)获取"pairObj[N]"的值
template <int N, typename T>
struct __RecursionPairValue;


// 获取__RecursionPair<T1, void>的"pairObj[0]"的值
template <typename T1>
struct __RecursionPairValue<0, __RecursionPair<T1, void>>
{
    // __Get函数的参数类型显然就是__RecursionPair<T1, void>
    typedef __RecursionPair<T1, void> __PairType;


    // __Get函数的返回值类型显然就是T1
    typedef T1 __ValueType;


    // 实际的返回值显然就是pairObj.__first
    static __ValueType &__Get(__PairType &pairObj) { return pairObj.__first; }
    static const __ValueType &__Get(const __PairType &pairObj) { return pairObj.__first; }
};
```

让我们继续。接下来实现\_\_RecursionPair\<T1, T2\>的取值：

``` Cpp
// 获取__RecursionPair<T1, T2>的"pairObj[0]"的值
template <typename T1, typename T2>
struct __RecursionPairValue<0, __RecursionPair<T1, T2>>
{
    // __Get函数的参数类型显然就是__RecursionPair<T1, T2>
    typedef __RecursionPair<T1, T2> __PairType;


    // __Get函数的返回值类型显然就是T1
    typedef T1 __ValueType;


    // 实际的返回值显然就是pairObj.__first
    static __ValueType &__Get(__PairType &pairObj) { return pairObj.__first; }
    static const __ValueType &__Get(const __PairType &pairObj) { return pairObj.__first; }
};


// 获取__RecursionPair<T1, T2>的"pairObj[1]"的值
template <typename T1, typename T2>
struct __RecursionPairValue<1, __RecursionPair<T1, T2>>
{
    // __Get函数的参数类型显然就是__RecursionPair<T1, T2>
    typedef __RecursionPair<T1, T2> __PairType;


    // __Get函数的返回值类型显然就是T2
    typedef T2 __ValueType;


    // 实际的返回值显然就是pairObj.__second
    static __ValueType &__Get(__PairType &pairObj) { return pairObj.__second; }
    static const __ValueType &__Get(const __PairType &pairObj) { return pairObj.__second; }
};
```

让我们继续。接下来实现\_\_RecursionPair\<T1, \_\_RecursionPair\<T2, T3\>\>的取值：

``` Cpp
// 获取__RecursionPair<T1, __RecursionPair<T2, T3>>的"pairObj[0]"的值
template <typename T1, typename T2, typename T3>
struct __RecursionPairValue<0, __RecursionPair<T1, __RecursionPair<T2, T3>>>
{
    // __Get函数的参数类型显然就是__RecursionPair<T1, __RecursionPair<T2, T3>>
    typedef __RecursionPair<T1, __RecursionPair<T2, T3>> __PairType;


    // __Get函数的返回值类型显然就是T1
    typedef T1 __ValueType;


    // 实际的返回值显然就是pairObj.__first
    static __ValueType &__Get(__PairType &pairObj) { return pairObj.__first; }
    static const __ValueType &__Get(const __PairType &pairObj) { return pairObj.__first; }
};


// 获取__RecursionPair<T1, __RecursionPair<T2, T3>>的"pairObj[1]"的值
template <typename T1, typename T2, typename T3>
struct __RecursionPairValue<1, __RecursionPair<T1, __RecursionPair<T2, T3>>>
{
    // __Get函数的参数类型显然就是__RecursionPair<T1, __RecursionPair<T2, T3>>
    typedef __RecursionPair<T1, __RecursionPair<T2, T3>> __PairType;


    // __Get函数的返回值类型显然就是T2
    typedef T2 __ValueType;


    // 实际的返回值显然就是pairObj.__second.__first
    static __ValueType &__Get(__PairType &pairObj) { return pairObj.__second.__first; }
    static const __ValueType &__Get(const __PairType &pairObj) { return pairObj.__second.__first; }
};
```

那么，如果N大于1，要怎么办呢？我们需要解决两个问题：

1. Get函数的返回值类型是什么？
2. 怎么得到"pairObj[N]"的值？

第一个问题的解决方案不言而喻：我们已经实现了可以获取到可递归Pair的任意位置的类型的工具，这当然可以在这里为我们所用；对于第二个问题，我们同样可以基于递归，对pairObj.second进行"拆分"，直至N降至1为止。请看以下示例：

``` Cpp
// 获取__RecursionPair<T1, __RecursionPair<T2, T3>>的"pairObj[N]"的值
template <int N, typename T1, typename T2, typename T3>
struct __RecursionPairValue<N, __RecursionPair<T1, __RecursionPair<T2, T3>>>
{
    // __Get函数的参数类型显然就是__RecursionPair<T1, __RecursionPair<T2, T3>>
    typedef __RecursionPair<T1, __RecursionPair<T2, T3>> __PairType;


    // __Get函数的返回值类型需要依赖我们前面已经实现的__RecursionPairType获取
    typedef typename __RecursionPairType<N, __PairType>::__ValueType __ValueType;


    // 我们并没有能力"拆分"pairObj.__second.__second，但是我们可以"寄希望于"递归
    static __ValueType &__Get(__PairType &pairObj)
    {
        return __RecursionPairValue<N - 1, __RecursionPair<T2, T3>>::__Get(pairObj.__second);
    }


    // 同上
    static const __ValueType &__Get(const __PairType &pairObj)
    {
        return __RecursionPairValue<N - 1, __RecursionPair<T2, T3>>::__Get(pairObj.__second);
    }
};
```

至此，我们就完整实现了根据"索引值"获取可递归Pair的某个位置的值这一工具。让我们来看看效果：

``` Cpp
int main()
{
    __RecursionPair<int, __RecursionPair<double, __RecursionPair<char, string>>> testPair;
    __RecursionPairValue<3, decltype(testPair)>::__Get(testPair) = "abc";
    cout << __RecursionPairValue<3, decltype(testPair)>::__Get(testPair);  // abc
}
```

同样，如果你觉得上述实现中的第4个和第5个特化（即\_\_RecursionPairValue\<0, \_\_RecursionPair\<T1, \_\_RecursionPair\<T2, T3\>\>\>和\_\_RecursionPairValue\<1, \_\_RecursionPair\<T1, \_\_RecursionPair\<T2, T3\>\>\>版本）是多余的，你可以去掉这些特化，然后编译试试看。

### 6.4 将"千层饼"擀成"单层饼"

本节将会是整个Tuple的实现中最为精彩的部分！

我们虽然已经实现了针对可递归Pair的取类型和取值工具，但我们还是没有实现出一个"扁平的"真正的Tuple（没错，终于又看到Tuple这个词了）。接下来，我们就开始着手考虑，如何将可递归Pair这张"千层饼"擀平，变成一张"单层饼"Tuple。

如何实现"擀平"这一操作呢？稍加思考不难发现，可递归Pair和Tuple之间似乎存在着这样的一一对应关系：

* 含有一个元素的Tuple，就是一个\_\_RecursionPair\<T1, void\>
* 含有两个元素的Tuple，就是一个\_\_RecursionPair\<T1, T2\>
* 含有三个元素的Tuple，就是一个\_\_RecursionPair\<T1, \_\_RecursionPair\<T2, T3\>\>
* 含有四个元素的Tuple，就是一个\_\_RecursionPair\<T1, \_\_RecursionPair\<T2, \_\_RecursionPair\<T3, T4\>\>\>
* ...

如何描述这种"是一个"的语义？哦！是继承！

请看以下示例：

``` Cpp
// 原型
// 通过Tuple<Types...>构造一个Tuple
template <typename... Types>
struct Tuple;


// 含有一个元素的Tuple，就是一个__RecursionPair<T1, void>
template <typename T1>
struct Tuple<T1>: __RecursionPair<T1, void>
{
    // 我是一个怎样的__RecursionPair？当然是继承的那个！
    typedef __RecursionPair<T1, void> __PairType;


    // 构造函数（待实现）
    Tuple();
    Tuple(const T1 &first);
};


// 含有两个元素的Tuple，就是一个__RecursionPair<T1, T2>
template <typename T1, typename T2>
struct Tuple<T1, T2>: __RecursionPair<T1, T2>
{
    // 我是一个怎样的__RecursionPair？当然也是继承的那个！
    typedef __RecursionPair<T1, T2> __PairType;


    // 构造函数（待实现）
    Tuple();
    Tuple(const T1 &first, const T2 &second);
};


// 默认构造函数
template <typename T1>
Tuple<T1>::Tuple() = default;


// 只需要调用Tuple的"可递归Pair形态"（即父类）的构造函数即可
template <typename T1>
Tuple<T1>::Tuple(const T1 &first): __PairType(first) {}


// 默认构造函数
template <typename T1, typename T2>
Tuple<T1, T2>::Tuple() = default;


// 同样，只需要调用Tuple的"可递归Pair形态"（即父类）的构造函数即可
template <typename T1, typename T2>
Tuple<T1, T2>::Tuple(const T1 &first, const T2 &second): __PairType(first, second) {}
```

那么，含有不止两个元素的Tuple，是哪个可递归Pair呢？如果你已经注意到了上面的两个Tuple实现中的"看似无用"的typedef，那么问题就能迎刃而解。这些typedef，保存了当前Tuple所对应的"可递归Pair形态"。从可递归Pair的角度去思考，不难找到以下规律：

* Tuple\<T1, T2, T3\>的"可递归Pair形态"是\_\_RecursionPair\<T1, typename Tuple\<T2, T3\>::\_\_PairType\>，即：将T1，与Tuple\<T2, T3\>的"可递归Pair形态"放入一个\_\_RecursionPair中，最终得到的结果是\_\_RecursionPair\<T1, \_\_RecursionPair\<T2, T3\>\>
* Tuple\<T1, T2, T3, T4\>的"可递归Pair形态"是\_\_RecursionPair\<T1, typename Tuple\<T2, T3, T4\>::\_\_PairType\>，即：将T1，与Tuple\<T2, T3, T4\>的"可递归Pair形态"放入一个\_\_RecursionPair中（Tuple\<T2, T3, T4\>的"可递归Pair形态"就是上面已经得到的Tuple\<T1, T2, T3\>的"可递归Pair形态"），最终得到的结果是\_\_RecursionPair\<T1, \_\_RecursionPair\<T2, \_\_RecursionPair\<T3, T4\>\>\>
* ...
* Tuple\<T1, Types...\>的"可递归Pair形态"是\_\_RecursionPair\<T1, typename Tuple\<Types...\>::\_\_PairType\>，即：将T1，与Tuple\<Types...\>的"可递归Pair形态"放入一个\_\_RecursionPair中，最终得到的结果是\_\_RecursionPair\<T1, \_\_RecursionPair\<T2, \_\_RecursionPair\<T3, ..., \_\_RecursionPair\<T(N - 1), T(N)\>\>\>...\>

找到了这一规律，代码实现也就轻而易举了。请看以下示例：

``` Cpp
// Tuple<T1, Types...>的"可递归Pair形态"是：将T1，与typename Tuple<Types...>::__PairType
//（即Tuple<Types...>的"可递归Pair形态"）放入一个__RecursionPair中
template <typename T1, typename... Types>
struct Tuple<T1, Types...>: __RecursionPair<T1, typename Tuple<Types...>::__PairType>
{
    // 我是一个怎样的__RecursionPair？同样也是继承的那个！
    typedef __RecursionPair<T1, typename Tuple<Types...>::__PairType> __PairType;


    // 构造函数（待实现）
    Tuple();
    Tuple(const T1 &first, const Types &... Args);
};
```

那么，这样的一个含有多个元素的Tuple，其构造函数又该如何实现呢？通过上文的讨论，我们不难发现：不管是什么样的Tuple（从只含有一个元素的Tuple到含有很多个元素的Tuple），其父类都是一个可递归Pair，而可递归Pair也是Pair，其构造函数永远只需要两个值（不管是多么复杂的可递归Pair）。所以，我们仍然可以通过直接调用父类的构造函数来对任意的Tuple进行构造。我们需要哪两个值来调用复杂的可递归Pair的构造函数呢？让我们继续进行"找规律"：

* 为了构造一个Tuple\<T1, T2, T3\>(Arg1, Arg2, Arg3)，我们需要构造一个typename Tuple\<T1, T2, T3\>::\_\_PairType，即一个\_\_RecursionPair\<T1, \_\_RecursionPair\<T2, T3\>\>，其中，T1来自于Arg1，而\_\_RecursionPair\<T2, T3\>可以通过构造一个Tuple\<T2, T3\>(Arg2, Arg3)得到（因为一个Tuple\<T2, T3\>就是一个\_\_RecursionPair\<T2, T3\>）
* 为了构造一个Tuple\<T1, T2, T3, T4\>(Arg1, Arg2, Arg3, Arg4)，我们需要构造一个typename Tuple\<T1, T2, T3, T4\>::\_\_PairType，即一个\_\_RecursionPair\<T1, \_\_RecursionPair\<T2, \_\_RecursionPair\<T3, T4\>\>\>，其中，T1同样来自于Arg1，而\_\_RecursionPair\<T2, \_\_RecursionPair\<T3, T4\>\>可以通过构造一个Tuple\<T2, T3, T4\>(Arg2, Arg3, Arg4)得到（因为一个Tuple\<T2, T3, T4\>就是一个\_\_RecursionPair\<T2, \_\_RecursionPair\<T3, T4\>\>）
* ...
* 为了构造一个Tuple\<T1, Types...\>(first, Args...)，我们需要构造一个typename Tuple\<T1, Types...\>::\_\_PairType，即一个\_\_RecursionPair\<T1, typename Tuple\<Types...\>::\_\_PairType\>，其中，T1来自于first，而typename Tuple\<Types...\>::\_\_PairType可以通过构造一个Tuple\<Types...\>(Args...)得到（因为一个Tuple\<Types...\>就是一个typename Tuple\<Types...\>::\_\_PairType）

我们再一次通过找规律的方法得到了结论！接下来就可以进行代码实现了。请看以下示例：

``` Cpp
// 默认构造函数
template <typename T1, typename... Types>
Tuple<T1, Types...>::Tuple() = default;


// 只需要调用Tuple的"可递归Pair形态"（即父类）的构造函数即可
// 构造__PairType的两个参数分别来自于first与构造Tuple<Types...>(Args...)所得到的一个__RecursionPair
template <typename T1, typename... Types>
Tuple<T1, Types...>::Tuple(const T1 &first, const Types &... Args):
    __PairType(first, Tuple<Types...>(Args...)) {}
```

至此，Tuple的实现中最重要的部分：Tuple的构造函数，也就全部实现完毕了。让我们立即来试用一下。请看以下示例：

``` Cpp
int main()
{
    Tuple<int, double, char, string> sampleTuple(1, 2., '3', "4");

    cout << __RecursionPairValue<0, decltype(sampleTuple)::__PairType>::__Get(sampleTuple) << endl;  // 1
    cout << __RecursionPairValue<1, decltype(sampleTuple)::__PairType>::__Get(sampleTuple) << endl;  // 2
    cout << __RecursionPairValue<2, decltype(sampleTuple)::__PairType>::__Get(sampleTuple) << endl;  // 3
    cout << __RecursionPairValue<3, decltype(sampleTuple)::__PairType>::__Get(sampleTuple) << endl;  // 4
}
```

由此可见，Tuple的构造函数工作正常（虽然我们暂时还只能通过"可递归Pair时代"的工具获取到其内部的值）。

### 6.5 Tuple的其他功能的实现

最后，就是一些简单的周边功能的实现了。

首先是MakeTuple快捷函数，此函数只需要使用一个可变参数模板封装Tuple的构造函数即可。请看以下示例：

``` Cpp
template <typename... Types>
inline Tuple<Types...> MakeTuple(const Types &... Args)
{
    return Tuple<Types...>(Args...);
}


int main()
{
    auto sampleTuple = MakeTuple(1, 2., '3');
}
```

然后是根据"索引值"获取Tuple的某个位置的类型的类，实现时只需要将全部操作直接委托给我们已经实现的\_\_RecursionPairType即可。请看以下示例：

``` Cpp
// 原型
// 通过typename TupleType<N, T>::Type获取"T[N]"的类型
template <int N, typename T>
struct TupleType;


// 仅当T是一个Tuple时，此类才有意义
template <int N, typename... Types>
struct TupleType<N, Tuple<Types...>>
{
    // 使用__RecursionPairType作用于Tuple<Types...>的"可递归Pair形态"上，就能获取"Tuple<Types...>[N]"的类型
    typedef typename __RecursionPairType<N, typename Tuple<Types...>::__PairType>::__ValueType Type;
};


int main()
{
    Tuple<int, double, char, string> sampleTuple(1, 2., '3', "4");

    cout << typeid(TupleType<0, decltype(sampleTuple)>::Type).name() << endl;  // int
    cout << typeid(TupleType<1, decltype(sampleTuple)>::Type).name() << endl;  // double
    cout << typeid(TupleType<2, decltype(sampleTuple)>::Type).name() << endl;  // char
    cout << typeid(TupleType<3, decltype(sampleTuple)>::Type).name() << endl;  // string
}
```

然后是根据"索引值"获取Tuple的某个位置的值的函数，实现时只需要将全部操作直接委托给我们已经实现的TupleType以及\_\_RecursionPairValue即可。请看以下示例：

``` Cpp
// 函数的返回值就是typename TupleType<N, Tuple<Types...>>::Type
template <int N, typename... Types>
inline typename TupleType<N, Tuple<Types...>>::Type &Get(Tuple<Types...> &tupleObj)
{
    // 使用__RecursionPairValue作用于Tuple<Types...>的"可递归Pair形态"上，就能获取"tupleObj[N]"的值
    return __RecursionPairValue<N, typename Tuple<Types...>::__PairType>::__Get(tupleObj);
}


// 同上
template <int N, typename... Types>
inline const typename TupleType<N, Tuple<Types...>>::Type &Get(const Tuple<Types...> &tupleObj)
{
    return __RecursionPairValue<N, typename Tuple<Types...>::__PairType>::__Get(tupleObj);
}


int main()
{
    Tuple<int, double, char, string> sampleTuple(1, 2., '3', "4");

    cout << Get<0>(sampleTuple) << endl;  // 1
    cout << Get<1>(sampleTuple) << endl;  // 2
    cout << Get<2>(sampleTuple) << endl;  // 3
    cout << Get<3>(sampleTuple) << endl;  // 4
}
```

最后是获取Tuple的长度的类，直接使用sizeof...(Types)即可。请看以下示例：

``` Cpp
// 原型
template <typename T>
struct TupleSize;


// 仅当T是一个Tuple时，此类才有意义
template <typename... Types>
struct TupleSize<Tuple<Types...>>
{
    // Tuple的长度显然就是Tuple的可变模板参数的数量
    static constexpr int Size = sizeof...(Types);
};


int main()
{
    cout << TupleSize<Tuple<int, double, char, string>>::Size << endl;  // 4
}
```

至此，Tuple的实现也就全部完成了。

### 6.6 本章后记

Tuple，作为一个看起来已然"突破极限"的高级容器，其背后的核心竟然只是一个"平淡无奇"的Pair，这不得不令人惊讶于基于模板的高阶抽象的威力。在Tuple的实现过程中，我们充分利用了模板偏特化，用以描绘出各种不同"形态"的可递归Pair；我们也使用了继承，用以描绘出Tuple与可递归Pair的一一对应关系。在这里，模板与继承，这两个"不同世界的产物"，被巧妙的结合在了一起，最终为我们带来了一场十分精彩的二重奏！

## 7 模板与高性能计算的极佳配合——表达式模板

表达式模板？什么？你没听说过？那就对了！通过本章的讨论，你就会了解到：模板是如何在用户无感知的前提下，将高性能计算引入我们的程序中的。

### 7.1 表达式的困境

让我们从一个看似很简单的问题开始：

```
如何实现向量的加法运算？
```

如果使用STL的array表示向量，不难做出以下实现：

``` Cpp
template <typename T, size_t N>
array<T, N> operator+(const array<T, N> &lhs, const array<T, N> &rhs)
{
    array<T, N> resArray;

    for (int idx = 0; idx < N; idx++) resArray[idx] = lhs[idx] + rhs[idx];

    return resArray;
}


int main()
{
    array<int, 3> lhs {1, 2, 3}, rhs {4, 5, 6}, res;

    res = lhs + rhs;

    for (auto val: res) cout << val << endl;  // 5 7 9
}
```

这个实现有什么问题呢？请看以下示例：

``` Cpp
lhs + rhs + lhs + rhs + lhs + rhs + lhs + rhs + lhs + rhs;  // 哦！大量的冗余计算！
```

在上面这个"10连加"表达式中，operator+函数一共被调用了9次，这也就意味着：函数体内的resArray临时变量被创建、return了9次（假设没有NRV优化），for循环也被执行了9次，而这还只是一次"10连加"所造成的结果。可想而知，在计算量变得越来越大时，这是多么大的时间耗费！此时，我们不难想到，上述的"10连加"表达式实际上能够被优化为如下实现：

``` Cpp
// 实际上只需要一次函数调用
template <typename T, size_t N>
array<T, N> operator+(const array<T, N> &lhs, const array<T, N> &rhs)
{
    array<T, N> resArray;

    // 实际上也只需要一次循环
    for (int idx = 0; idx < N; idx++)
    {
        // 只需要在循环体内执行"10连加"即可
        resArray[idx] = lhs[idx] + rhs[idx] + lhs[idx] + rhs[idx] + lhs[idx] +
            rhs[idx] + lhs[idx] + rhs[idx] + lhs[idx] + rhs[idx];
    }

    return resArray;
}
```

可问题是，编译器就算有能力优化成这样的实现，其也不能优化成这样。这是由于C++的表达式语义本就是"积极主动的"，当编译器看到lhs + rhs...时，其就必须遵守C++的语义规定，立即计算此加法，而"暂且不顾"后续表达式。

看来，编译器优化是彻底不可能帮得上忙了。这让我们陷入了困境之中。

### 7.2 一个天马行空的想法

既然编译器帮不上忙，那我们是否能通过某种技术，"绕过"编译器的这种主动计算呢？如果你的想象力足够丰富，也许你会有这样的想法：

```
能否将表达式看作某种"字符串"，这样，加法就相当于"字符串的拼接"呢？而当我们真的需要表达式的结果时，我们可以实现一个对"表达式字符串"进行求值的函数来进行求值。
```

这是一个天马行空的想法，但基于模板，这个想法是真的可以实现的！这就是本章将要讨论的表达式模板技术。

### 7.3 向量类的实现

首先，让我们实现一个Array类，用于存放一个向量。请看以下示例：

``` Cpp
template <typename T, int N>
class __Array
{
public:

    // 构造函数
    __Array();
    explicit __Array(const T &val);
    __Array(initializer_list<T> initializerList);


    // operator[]
    T &operator[](int idx) { return __data[idx]; }
    const T &operator[](int idx) const { return __data[idx]; }


private:

    // 一个C语言数组，用于存放向量
    T __data[N];
};


template <typename T, int N>
__Array<T, N>::__Array() = default;


template <typename T, int N>
__Array<T, N>::__Array(const T &val)
{
    for (int idx = 0; idx < N; idx++)
    {
        __data[idx] = val;
    }
}


template <typename T, int N>
__Array<T, N>::__Array(initializer_list<T> initializerList)
{
    int idx = 0;

    for (auto &val: initializerList)
    {
        __data[idx++] = val;
    }
}


int main()
{
    __Array<int, 3> lhs {1, 2, 3};

    for (int idx = 0; idx < 3; idx++) cout << lhs[idx] << endl;  // 1 2 3
}
```

我们为这个Array实现了默认构造函数，Fill构造函数，initializer\_list构造函数，以及operator[]重载。看上去平淡无奇，不是吗？

### 7.4 "表达式字符串"的实现

接下来，我们就来实现上文中的"表达式字符串"（当然，我们不是真的去实现一个特殊的字符串）。一个"表达式字符串"，如"lhs + rhs"，是由哪几部分组成的呢？显然，其是由lhs、"+"以及rhs组成，其中，lhs与rhs代表的是某个值，而"+"代表的是一个动作。如果我们使用两个变量分别存放lhs与rhs，并使用一个函数表达"+"这一动作，我们就能够实现出一个"表达式字符串"了。而将这些内容封装进一个模板中，我们也就得到了一个"表达式模板"。请看以下示例：

``` Cpp
// 加法表达式模板
template <typename T, typename LExpr, typename RExpr>
class __Plus
{
public:

    // 构造函数
    __Plus(const LExpr &lhs, const RExpr &rhs);


    // 当对这个表达式模板进行[...]运算的时候，就能得到这个表达式模板在某个"索引值"位置上的加法计算的结果
    // 也就是说，表达式模板也是某种从外观上看和向量别无二致的东西
    T operator[](int idx) const { return __lhs[idx] + __rhs[idx]; }


private:

    // 用于保存LExpr与RExpr的引用的数据成员
    const LExpr &__lhs;
    const RExpr &__rhs;
};


template <typename T, typename LExpr, typename RExpr>
__Plus<T, LExpr, RExpr>::__Plus(const LExpr &lhs, const RExpr &rhs):
    __lhs(lhs), __rhs(rhs) {}
```

\_\_Plus的模板参数包含加法的返回值类型T，以及左右值类型LExpr和RExpr；在\_\_Plus中，我们声明了两个分别指向LExpr和RExpr的引用；而在构造函数中，lhs、rhs被分别绑定至类中的两个引用上，此时，我们并没有执行任何加法运算。那么，什么时候才执行加法运算呢？从代码中不难看出，直到operator[]时，才会真正计算加法。

这是一种利用模板实现的"惰性计算"技术，当加法语义出现时，我们并没有真的执行加法，而只是执行了一次成本很低的"记录"操作，我们记录了执行一次加法所需要的全部信息：左值、加这个动作、以及右值。仅当真正需要加法的结果时，\_\_Plus才会"在我们强硬的驱使下"计算加法。并且，就算是在这种"强硬的驱使下"，\_\_Plus每次也只会计算一个位置的加法。这就使得\_\_Plus能够最大程度的规避无意义的加法计算（设想我们进行了一次十万维向量的加法，但我们只需要知道第五万维这一个位置的加法结果）。

此外，\_\_Plus在设计上刻意的模仿了\_\_Array的操作，这就使得\_\_Plus也能够像一个\_\_Array那样具有"索引值"。这样做的意义是什么呢？仅仅是为了方便、美观吗？我们将在下一节中揭晓答案。

接下来，让我们试着使用一下\_\_Plus类，体验一下这种"惰性计算"技术。请看以下示例：

``` Cpp
int main()
{
    __Array<int, 3> lhs {1, 2, 3}, rhs {4, 5, 6};

    // 保存了lhs + rhs这个表达式，但不对其进行计算
    __Plus<int, __Array<int, 3>, __Array<int, 3>> res(lhs, rhs);

    for (int idx = 0; idx < 3; idx++)
    {
        // 这里才计算加法
        cout << res[idx] << endl;
    }
}
```

看到这里，也许你会恍然大悟："哦！这个\_\_Plus和上一章的可递归Pair一样，也是可以递归的！"请看以下示例：

``` Cpp
int main()
{
    __Array<int, 3> lhs {1, 2, 3}, rhs {4, 5, 6};

    // 保存了lhs + rhs + lhs这个表达式，但不对其进行计算
    // 可是这也太反人类了吧！
    __Plus<int, __Array<int, 3>, __Array<int, 3>> tmp(lhs, rhs);
    __Plus<int, __Plus<int, __Array<int, 3>, __Array<int, 3>>, __Array<int, 3>> res(tmp, lhs);

    for (int idx = 0; idx < 3; idx++)
    {
        // 这里才计算加法
        cout << res[idx] << endl;
    }
}
```

我们通过整整两行的"超长"代码，"终于"实现了lhs + rhs + lhs的惰性加法。显然，这样的实现是非常"反人类"的，有什么办法能对其进行简化，甚至让用户无感知呢？稍加思索就能够发现，只要使用运算符重载，我们就能把所有这些都隐藏于幕后，只留下lhs + rhs + lhs本身。请看以下示例：

``` Cpp
template </* 这里写什么？*/>
/* 这里写什么？*/ operator+(const /* 这里写什么？*/ &lhs, const /* 这里写什么？*/ &rhs)
{
    return /* 这里写什么？*/;
}


int main()
{
    /* 这里写什么？*/ lhs {1, 2, 3}, rhs {4, 5, 6}, res;

    // 最终的效果，太棒了！
    res = lhs + rhs + lhs;

    for (int idx = 0; idx < 3; idx++)
    {
        cout << res[idx] << endl;
    }
}
```

对于operator+，其需要同时满足"\_\_Array + \_\_Array"、"\_\_Array + \_\_Plus"、"\_\_Plus + \_\_Array"、"\_\_Plus + \_\_Plus"等等的"排列组合"（并且，请不要忘了：除了"加"，还有"减乘除"呢！）。这就使得我们难以确定lhs与rhs的类型。难道真的要为每种情况都写一个operator+重载吗？请接着往下看。

### 7.5 再加一层抽象

如何规避这种"排列组合"呢？让我们开拓一下思维，不难发现：单独的一个\_\_Array是一个表达式，而\_\_Plus（任意两个表达式相加的结果）也是一个表达式，并且他们的共性即在于，都是可以基于operator[]进行表达式求值的。至此，解决方案水落石出：我们可以在\_\_Array和\_\_Plus之上再增加一个抽象层，表达"表达式"语义，而\_\_Array和\_\_Plus在此抽象层中并无区别，都是一个可以进行operator[]运算的"表达式"。此时你应该能够明白：为什么\_\_Plus要"刻意"模仿\_\_Array的operator[]了。请看以下示例：

``` Cpp
template <typename T, int N, typename Expr>
class __Expression
{
public:

    // 适用于__Array的构造函数
    __Expression();
    explicit __Expression(const T &val);
    __Expression(initializer_list<T> initializerList);


    // 适用于__Plus的构造函数
    __Expression(const Expr &expr);


    // operator[]直接委托给__expr执行
    T &operator[](int idx) { return __expr[idx]; }
    T operator[](int idx) const { return __expr[idx]; }


    // operator=
    template <typename RExpr>
    __Expression &operator=(const __Expression<T, N, RExpr> &rhs);


private:

    // __expr可能是一个__Array，也可能是一个__Plus
    Expr __expr;
};


template <typename T, int N, typename Expr>
__Expression<T, N, Expr>::__Expression() = default;


template <typename T, int N, typename Expr>
__Expression<T, N, Expr>::__Expression(const T &val): __expr(val) {}


template <typename T, int N, typename Expr>
__Expression<T, N, Expr>::__Expression(initializer_list<T> initializerList): __expr(initializerList) {}


template <typename T, int N, typename Expr>
__Expression<T, N, Expr>::__Expression(const Expr &expr): __expr(expr) {}


// operator=直接委托给__expr执行
// 直到operator=发生时，rhs才会真正被计算
template <typename T, int N, typename Expr>
template <typename RhsExpr>
__Expression<T, N, Expr> &__Expression<T, N, Expr>::operator=(
    const __Expression<T, N, RhsExpr> &rhs)
{
    for (int idx = 0; idx < N; idx++)
    {
        // 计算rhs[idx]的值，并赋值给左值
        __expr[idx] = rhs[idx];
    }

    return *this;
}
```

让我们来分析这一实现：既然我们需要将\_\_Array和\_\_Plus都抽象为一个表达式，那么我们就可以增加一个模板参数Expr，用以标明这个\_\_Expression到底是什么（是\_\_Array还是\_\_Plus）。由于Expr既可以是\_\_Array又可以是\_\_Plus，我们就需要实现多个构造函数，使得这两种类型的值都可以在\_\_Expression中构造。所以，我们实现了三个和\_\_Array的三个构造函数功能一致的构造函数，以及可以使用一个Expr作为参数的构造函数。然后，我们将operator[]和operator=都直接委托给\_\_expr执行。

显然，当用户在使用的时候，\_\_Expression的Expr模板参数必须是\_\_Array，所以我们可以声明一个固定了Expr模板参数的模板，作为面向用户的Array接口。请看以下示例：

``` Cpp
// 最终面向用户的Array接口
template <typename T, int N>
using Array = __Expression<T, N, __Array<T, N>>;
```

同时，作为实现者，我们也可以创造一些更复杂的Expr模板参数。所以，就让我们来实现上一节中未能实现的operator+吧。请看以下示例：

``` Cpp
// __Expression<T, N, LExpr> + __Expression<T, N, RExpr>的结果是一个新的__Expression
// 其第一、二模板参数不变，第三模板参数是LExpr + RExpr的结果，即__Plus<T, LExpr, RExpr>
template <typename T, int N, typename LExpr, typename RExpr>
inline __Expression<T, N, __Plus<T, LExpr, RExpr>> operator+(
    const __Expression<T, N, LExpr> &lhs, const __Expression<T, N, RExpr> &rhs)
{
    // 用lhs与rhs的__expr，构造出一个__Plus
    // 再用这个__Plus，构造出一个新的__Expression（使用的是__Expression的第四构造函数）
    return __Expression<T, N, __Plus<T, LExpr, RExpr>>(
        __Plus<T, LExpr, RExpr>(lhs.__expr, rhs.__expr));
}
```

看上去很复杂的样子？让我们来分析一下。首先，由于我们并不知道lhs和rhs的Expr分别是什么（二者都可能是\_\_Array，如果这是一个"新的"Array；或\_\_Plus，如果这已经是一个表达式），所以我们需要两个模板参数LExpr与RExpr，以分别代表lhs和rhs的Expr类型；但同时我们知道，只有相同类型的Array之间可以进行运算，所以我们只需要一套T与N即可。所以，两个形参分别是const Array\<T, N, LExpr\> &lhs与const Array\<T, N, RExpr\> &rhs。

返回值是什么呢？不难发现，当\_\_Expression\<T, N, LExpr\>与\_\_Expression\<T, N, RExpr\>相加后，结果仍然是一个新的\_\_Expression，而真正需要相加的其实是LExpr与RExpr，且相加的结果是\_\_Plus\<T, LExpr, RExpr\>（事实上，相加的结果也可以就是\_\_Plus\<T, \_\_Expression\<T, N, LExpr\>, \_\_Expression\<T, N, RExpr\>\>，你一定不难想出其中原因。但很明显，这样的代码也实在是太长，太反人类了），故返回值的类型就是一个Expr模板参数为\_\_Plus\<T, LExpr, RExpr\>的\_\_Expression，即\_\_Expression\<T, N, \_\_Plus\<T, LExpr, RExpr\>\>，而实际的返回值需要先使用lhs与rhs的\_\_expr，构造出一个\_\_Plus，再用这个\_\_Plus，构造出一个新的\_\_Expression。这里使用的是\_\_Expression的第四构造函数。

现在，让我们试着使用一下我们刚刚实现的Array（请注意：如果你现在就实际编译以下这段代码，请去除\_\_Expression的\_\_expr的private限定，或为operator+函数进行友元授权）。请看以下示例：

``` Cpp
int main()
{
    // lhs，rhs，res的实际类型都是__Expression<int, 3, __Array<int, 3>>
    Array<int, 3> lhs {1, 2, 3}, rhs {4, 5, 6}, res;

    // "什么都不做"（不会进行实际的运算）
    lhs + rhs + lhs;

    // lhs + rhs...的类型是__Expression<int, 3, __Plus<__Array<int, 3>, __Array<int, 3>>>
    // ... + lhs的类型是__Expression<int, 3, __Plus<__Plus<__Array<int, 3>, __Array<int, 3>>, __Array<int, 3>>>
    // 直到operator=发生时，才会进行实际的运算
    res = lhs + rhs + lhs;

    for (int idx = 0; idx < 3; idx++)
    {
        // 此时，res[idx]就是一次指针运算
        cout << res[idx] << endl;
    }
}
```

观察上述代码不难发现，我们仅仅才做了两次加法，\_\_Expression的类型就已经"长的没法看"了。但是事实上，我们根本就不用关心\_\_Expression的类型究竟是什么，而只需要牢记以下两点即可：

1. 不管进行多少次计算，或一次计算都还没有进行（即一个Array），我们所操作的都是一个（可能具有很复杂类型的）\_\_Expression
2. 对\_\_Expression进行operator[]，就相当于对\_\_Expression中的\_\_expr进行operator[]；而\_\_expr无非只有两种情况：其要么是一个\_\_Array，此时，operator[]就相当于一次数组取值；要么是一个\_\_Plus，此时，operator[]就相当于递归地调用lhs[idx] + rhs[idx]，直到lhs与rhs都不再是一个\_\_Plus为止

由此可见，\_\_Expression一方面为用户提供了对于表达式模板的存在无感知的Array类，另一方面又丝毫不丢失其作为表达式模板的功能，实在是一个优秀的"左右开弓"类；另一方面，由于我们的\_\_Array与\_\_Plus均实现了统一的operator[]接口，这就使得\_\_Expression能够"自适应地"最终实现对其自身的求值。以上种种，都能够为我们展现出"抽象"这一思想的精彩之处。

### 7.6 让标量也加入进来

在数学中，一个向量不仅可以和另一个向量相加，还可以和一个标量（即一个T类型的值）相加。本节我们就来实现这一功能。

如何让标量也加入我们的"\_\_Expression大家族"中呢？没错，关键就在于我们在上几节已经"老生常谈"的operator[]。虽然标量根本就没有operator[]这一概念，我们也可以"强行的"为其添加这一概念，以使其适配\_\_Expression的需要。请看以下示例：

``` Cpp
// 为标量提供的封装类，从而使得标量也能够适配__Expression
template <typename T>
class __Scalar
{
public:

    // 构造函数
    __Scalar(T val);


    // 强行为标量提供一个"莫名其妙的"operator[]
    // 不管索引值是多少（事实上我们根本就无视了这个索引值），都返回val
    T operator[](int) const { return __val; }


private:

    // 使用一个T类型的数据成员存放构造函数中的val
    T __val;
};


template <typename T>
__Scalar<T>::__Scalar(T val): __val(val) {}
```

此时，\_\_Expression的Expr模板参数就不仅可以是\_\_Array或\_\_Plus，还可以是\_\_Scalar了。

让我们继续，实现适用于\_\_Expression与\_\_Scalar之间的加法的运算符重载。请看以下示例：

``` Cpp
// __Expression<T, N, LExpr> + T
// 其结果为__Plus<T, LExpr, __Scalar<T>>
template <typename T, int N, typename LExpr>
inline __Expression<T, N, __Plus<T, LExpr, __Scalar<T>>> operator+(
    const __Expression<T, N, LExpr> &lhs, const T &rhs)
{
    // 先使用rhs构造出一个__Scalar<T>
    // 再使用lhs的__expr和__Scalar<T>构造出一个__Plus
    // 最后使用一个__Plus构造出一个新的__Expression
    return __Expression<T, N, __Plus<T, LExpr, __Scalar<T>>>(
        __Plus<T, LExpr, __Scalar<T>>(lhs.__expr, __Scalar<T>(rhs)));
}


// T + __Expression<T, N, RExpr>
// 其结果为：__Plus<T, __Scalar<T>, RExpr>
template <typename T, int N, typename RExpr>
inline __Expression<T, N, __Plus<T, __Scalar<T>, RExpr>> operator+(
    const T &lhs, const __Expression<T, N, RExpr> &rhs)
{
    // 先使用lhs构造出一个__Scalar<T>
    // 再使用__Scalar<T>和rhs的__expr构造出一个__Plus
    // 最后使用一个__Plus构造出一个新的__Expression
    return __Expression<T, N, __Plus<T, __Scalar<T>, RExpr>>(
        __Plus<T, __Scalar<T>, RExpr>(__Scalar<T>(lhs), rhs.__expr));
}
```

在我们试用这一功能前，其实还有一件事是没有完成的。请设想：当我们写下"lhs + 1"这一表达式时，这里的"1"显然是一个临时量，而如果使用我们现在所实现的\_\_Plus，那么这个临时量"1"将被存入一个引用中。这将立即导致"悬挂引用"的发生！所以，我们需要实现一个简单的Traits类，在面对一个\_\_Scalar时，将\_\_Plus中的数据成员类型，从引用类型自动切换至值类型。这一Traits的实现非常简单，请看以下示例：

``` Cpp
// 不管T是是什么，都萃取出一个const T &类型...
template <typename T>
struct __ScalarTypeTraits
{
    typedef const T &__Type;
};


// ...但是，如果T是一个__Scalar<T>类型，则萃取出一个__Scalar<T>类型
template <typename T>
struct __ScalarTypeTraits<__Scalar<T>>
{
    typedef __Scalar<T> __Type;
};
```

有了这个Traits，我们就可以使用这个Traits改进我们的\_\_Plus类了。请看以下示例：

``` Cpp
template <typename T, typename LExpr, typename RExpr>
class __Plus
{

// ...


private:

    // 原实现：
    // const LExpr &__lhs;
    // const RExpr &__rhs;

    // 改进后的实现：
    // 在LExpr（或RExpr）为__Scalar<LExpr>（或__Scalar<RExpr>）时
    // __Type将从引用类型自动切换至值类型
    typename __ScalarTypeTraits<LExpr>::__Type __lhs;
    typename __ScalarTypeTraits<RExpr>::__Type __rhs;
};
```

至此，我们就可以将标量也加入到表达式模板中了（请注意：如果你现在就实际编译以下这段代码，请去除\_\_Expression的\_\_expr的private限定，或为operator+函数进行友元授权）。请看以下示例：

``` Cpp
int main()
{
    Array<int, 3> lhs {1, 2, 3}, rhs {4, 5, 6}, res;

    // 加入标量
    res = lhs + rhs + lhs + 1;

    // 7 10 13
    for (int idx = 0; idx < 3; idx++)
    {
        cout << res[idx] << endl;
    }
}
```

本节中，我们通过一个对标量的简单的封装类，使得标量也能够加入到表达式模板中；同时，为了避免标量临时量所引发的"悬挂引用"问题，我们又实现了一个简单的Traits类，用于在面对标量时自动将表达式模板中的引用类型切换为值类型。

至此，表达式模板的全部技术就都讨论完毕了。下一节，我们将最终给出表达式模板的完整实现。

### 7.7 完整的实现

由于本章的代码较为分散，且我们仍有很多重复性的代码没有于上文中给出。故本节中，我们将给出表达式模板的完整实现。主要包含以下几点新增内容：

1. 我们不仅需要实现operator+，还要实现operator-、operator\*、operator/和operator%。这些实现都是operator+的简单重复
2. 我们需要为所有的operator函数添加友元授权
3. 我们需要为\_\_Expression实现operator\<\<重载（实现方案与operator=一致）

请看以下示例：

``` Cpp
// "梦开始的地方"：__Array类
template <typename T, int N>
class __Array
{
public:

    // 构造函数
    __Array();
    explicit __Array(const T &val);
    __Array(initializer_list<T> initializerList);


    // operator[]
    T &operator[](int idx) { return __data[idx]; }
    const T &operator[](int idx) const { return __data[idx]; }


private:

    // 数据成员
    T __data[N];
};


template <typename T, int N>
__Array<T, N>::__Array() = default;


template <typename T, int N>
__Array<T, N>::__Array(const T &val)
{
    for (int idx = 0; idx < N; idx++)
    {
        __data[idx] = val;
    }
}


template <typename T, int N>
__Array<T, N>::__Array(initializer_list<T> initializerList)
{
    int idx = 0;

    for (auto &val: initializerList)
    {
        __data[idx++] = val;
    }
}


// 标量适配器
template <typename T>
class __Scalar
{
public:

    // 构造函数
    __Scalar(T val);


    // operator[]
    T operator[](int) const { return __val; }


private:

    // 数据成员
    T __val;
};


template <typename T>
__Scalar<T>::__Scalar(T val): __val(val) {}


// 标量值类型萃取器
template <typename T>
struct __ScalarTypeTraits
{
    typedef const T &__Type;
};


template <typename T>
struct __ScalarTypeTraits<__Scalar<T>>
{
    typedef __Scalar<T> __Type;
};


// 加法表达式模板
template <typename T, typename LExpr, typename RExpr>
class __Plus
{
public:

    // 构造函数
    __Plus(const LExpr &lhs, const RExpr &rhs);


    // operator[]
    T operator[](int idx) const { return __lhs[idx] + __rhs[idx]; }


private:

    // 数据成员
    typename __ScalarTypeTraits<LExpr>::__Type __lhs;
    typename __ScalarTypeTraits<RExpr>::__Type __rhs;
};


// 减法表达式模板
template <typename T, typename LExpr, typename RExpr>
class __Minus
{
public:

    // 构造函数
    __Minus(const LExpr &lhs, const RExpr &rhs);


    // operator[]
    T operator[](int idx) const { return __lhs[idx] - __rhs[idx]; }


private:

    // 数据成员
    typename __ScalarTypeTraits<LExpr>::__Type __lhs;
    typename __ScalarTypeTraits<RExpr>::__Type __rhs;
};


// 乘法表达式模板
template <typename T, typename LExpr, typename RExpr>
class __Multiplies
{
public:

    // 构造函数
    __Multiplies(const LExpr &lhs, const RExpr &rhs);


    // operator[]
    T operator[](int idx) const { return __lhs[idx] * __rhs[idx]; }


private:

    // 数据成员
    typename __ScalarTypeTraits<LExpr>::__Type __lhs;
    typename __ScalarTypeTraits<RExpr>::__Type __rhs;
};


// 除法表达式模板
template <typename T, typename LExpr, typename RExpr>
class __Divides
{
public:

    // 构造函数
    __Divides(const LExpr &lhs, const RExpr &rhs);


    // operator[]
    T operator[](int idx) const { return __lhs[idx] / __rhs[idx]; }


private:

    // 数据成员
    typename __ScalarTypeTraits<LExpr>::__Type __lhs;
    typename __ScalarTypeTraits<RExpr>::__Type __rhs;
};


// 取模表达式模板
template <typename T, typename LExpr, typename RExpr>
class __Modulus
{
public:

    // 构造函数
    __Modulus(const LExpr &lhs, const RExpr &rhs);


    // operator[]
    T operator[](int idx) const { return __lhs[idx] % __rhs[idx]; }


private:

    // 数据成员
    typename __ScalarTypeTraits<LExpr>::__Type __lhs;
    typename __ScalarTypeTraits<RExpr>::__Type __rhs;
};


template <typename T, typename LExpr, typename RExpr>
__Plus<T, LExpr, RExpr>::__Plus(const LExpr &lhs, const RExpr &rhs): __lhs(lhs), __rhs(rhs) {}


template <typename T, typename LExpr, typename RExpr>
__Minus<T, LExpr, RExpr>::__Minus(const LExpr &lhs, const RExpr &rhs): __lhs(lhs), __rhs(rhs) {}


template <typename T, typename LExpr, typename RExpr>
__Multiplies<T, LExpr, RExpr>::__Multiplies(const LExpr &lhs, const RExpr &rhs): __lhs(lhs), __rhs(rhs) {}


template <typename T, typename LExpr, typename RExpr>
__Divides<T, LExpr, RExpr>::__Divides(const LExpr &lhs, const RExpr &rhs): __lhs(lhs), __rhs(rhs) {}


template <typename T, typename LExpr, typename RExpr>
__Modulus<T, LExpr, RExpr>::__Modulus(const LExpr &lhs, const RExpr &rhs): __lhs(lhs), __rhs(rhs) {}


// __Expression表达式类
template <typename T, int N, typename Expr>
class __Expression
{
public:

    // 构造函数
    __Expression();
    explicit __Expression(const T &val);
    __Expression(initializer_list<T> initializerList);
    __Expression(const Expr &expr);


    // operator[]
    T &operator[](int idx) { return __expr[idx]; }
    T operator[](int idx) const { return __expr[idx]; }


    // operator=
    template <typename RExpr>
    __Expression &operator=(const __Expression<T, N, RExpr> &rhs);


private:

    // 数据成员
    Expr __expr;


// 以下均为友元授权

// operator+
template <typename T_, int N_, typename LExpr, typename RExpr>
friend inline __Expression<T_, N_, __Plus<T_, LExpr, RExpr>> operator+(
    const __Expression<T_, N_, LExpr> &lhs, const __Expression<T_, N_, RExpr> &rhs);


template <typename T_, int N_, typename LExpr>
friend inline __Expression<T_, N_, __Plus<T_, LExpr, __Scalar<T_>>> operator+(
    const __Expression<T_, N_, LExpr> &lhs, const T_ &rhs);


template <typename T_, int N_, typename RExpr>
friend inline __Expression<T_, N_, __Plus<T_, __Scalar<T_>, RExpr>> operator+(
    const T_ &lhs, const __Expression<T_, N_, RExpr> &rhs);


// operator-
template <typename T_, int N_, typename LExpr, typename RExpr>
friend inline __Expression<T_, N_, __Minus<T_, LExpr, RExpr>> operator-(
    const __Expression<T_, N_, LExpr> &lhs, const __Expression<T_, N_, RExpr> &rhs);


template <typename T_, int N_, typename LExpr>
friend inline __Expression<T_, N_, __Minus<T_, LExpr, __Scalar<T_>>> operator-(
    const __Expression<T_, N_, LExpr> &lhs, const T_ &rhs);


template <typename T_, int N_, typename RExpr>
friend inline __Expression<T_, N_, __Minus<T_, __Scalar<T_>, RExpr>> operator-(
    const T_ &lhs, const __Expression<T_, N_, RExpr> &rhs);


// operator*
template <typename T_, int N_, typename LExpr, typename RExpr>
friend inline __Expression<T_, N_, __Multiplies<T_, LExpr, RExpr>> operator*(
    const __Expression<T_, N_, LExpr> &lhs, const __Expression<T_, N_, RExpr> &rhs);


template <typename T_, int N_, typename LExpr>
friend inline __Expression<T_, N_, __Multiplies<T_, LExpr, __Scalar<T_>>> operator*(
    const __Expression<T_, N_, LExpr> &lhs, const T_ &rhs);


template <typename T_, int N_, typename RExpr>
friend inline __Expression<T_, N_, __Multiplies<T_, __Scalar<T_>, RExpr>> operator*(
    const T_ &lhs, const __Expression<T_, N_, RExpr> &rhs);


// operator/
template <typename T_, int N_, typename LExpr, typename RExpr>
friend inline __Expression<T_, N_, __Divides<T_, LExpr, RExpr>> operator/(
    const __Expression<T_, N_, LExpr> &lhs, const __Expression<T_, N_, RExpr> &rhs);


template <typename T_, int N_, typename LExpr>
friend inline __Expression<T_, N_, __Divides<T_, LExpr, __Scalar<T_>>> operator/(
    const __Expression<T_, N_, LExpr> &lhs, const T_ &rhs);


template <typename T_, int N_, typename RExpr>
friend inline __Expression<T_, N_, __Divides<T_, __Scalar<T_>, RExpr>> operator/(
    const T_ &lhs, const __Expression<T_, N_, RExpr> &rhs);


// operator%
template <typename T_, int N_, typename LExpr, typename RExpr>
friend inline __Expression<T_, N_, __Modulus<T_, LExpr, RExpr>> operator%(
    const __Expression<T_, N_, LExpr> &lhs, const __Expression<T_, N_, RExpr> &rhs);


template <typename T_, int N_, typename LExpr>
friend inline __Expression<T_, N_, __Modulus<T_, LExpr, __Scalar<T_>>> operator%(
    const __Expression<T_, N_, LExpr> &lhs, const T_ &rhs);


template <typename T_, int N_, typename RExpr>
friend inline __Expression<T_, N_, __Modulus<T_, __Scalar<T_>, RExpr>> operator%(
    const T_ &lhs, const __Expression<T_, N_, RExpr> &rhs);
};


template <typename T, int N, typename Expr>
__Expression<T, N, Expr>::__Expression() = default;


template <typename T, int N, typename Expr>
__Expression<T, N, Expr>::__Expression(const T &val): __expr(val) {}


template <typename T, int N, typename Expr>
__Expression<T, N, Expr>::__Expression(initializer_list<T> initializerList): __expr(initializerList) {}


template <typename T, int N, typename Expr>
__Expression<T, N, Expr>::__Expression(const Expr &expr): __expr(expr) {}


template <typename T, int N, typename Expr>
template <typename RhsExpr>
__Expression<T, N, Expr> &__Expression<T, N, Expr>::operator=(
    const __Expression<T, N, RhsExpr> &rhs)
{
    for (int idx = 0; idx < N; idx++)
    {
        __expr[idx] = rhs[idx];
    }

    return *this;
}


// 运算符重载

// __Expression + __Expression
template <typename T, int N, typename LExpr, typename RExpr>
inline __Expression<T, N, __Plus<T, LExpr, RExpr>> operator+(
    const __Expression<T, N, LExpr> &lhs, const __Expression<T, N, RExpr> &rhs)
{
    return __Expression<T, N, __Plus<T, LExpr, RExpr>>(
        __Plus<T, LExpr, RExpr>(lhs.__expr, rhs.__expr));
}


// __Expression + __Scalar
template <typename T, int N, typename LExpr>
inline __Expression<T, N, __Plus<T, LExpr, __Scalar<T>>> operator+(
    const __Expression<T, N, LExpr> &lhs, const T &rhs)
{
    return __Expression<T, N, __Plus<T, LExpr, __Scalar<T>>>(
        __Plus<T, LExpr, __Scalar<T>>(lhs.__expr, __Scalar<T>(rhs)));
}


// __Scalar + __Expression
template <typename T, int N, typename RExpr>
inline __Expression<T, N, __Plus<T, __Scalar<T>, RExpr>> operator+(
    const T &lhs, const __Expression<T, N, RExpr> &rhs)
{
    return __Expression<T, N, __Plus<T, __Scalar<T>, RExpr>>(
        __Plus<T, __Scalar<T>, RExpr>(__Scalar<T>(lhs), rhs.__expr));
}


// __Expression - __Expression
template <typename T, int N, typename LExpr, typename RExpr>
inline __Expression<T, N, __Minus<T, LExpr, RExpr>> operator-(
    const __Expression<T, N, LExpr> &lhs, const __Expression<T, N, RExpr> &rhs)
{
    return __Expression<T, N, __Minus<T, LExpr, RExpr>>(
        __Minus<T, LExpr, RExpr>(lhs.__expr, rhs.__expr));
}


// __Expression - __Scalar
template <typename T, int N, typename LExpr>
inline __Expression<T, N, __Minus<T, LExpr, __Scalar<T>>> operator-(
    const __Expression<T, N, LExpr> &lhs, const T &rhs)
{
    return __Expression<T, N, __Minus<T, LExpr, __Scalar<T>>>(
        __Minus<T, LExpr, __Scalar<T>>(lhs.__expr, __Scalar<T>(rhs)));
}


// __Scalar - __Expression
template <typename T, int N, typename RExpr>
inline __Expression<T, N, __Minus<T, __Scalar<T>, RExpr>> operator-(
    const T &lhs, const __Expression<T, N, RExpr> &rhs)
{
    return __Expression<T, N, __Minus<T, __Scalar<T>, RExpr>>(
        __Minus<T, __Scalar<T>, RExpr>(__Scalar<T>(lhs), rhs.__expr));
}


// __Expression * __Expression
template <typename T, int N, typename LExpr, typename RExpr>
inline __Expression<T, N, __Multiplies<T, LExpr, RExpr>> operator*(
    const __Expression<T, N, LExpr> &lhs, const __Expression<T, N, RExpr> &rhs)
{
    return __Expression<T, N, __Multiplies<T, LExpr, RExpr>>(
        __Multiplies<T, LExpr, RExpr>(lhs.__expr, rhs.__expr));
}


// __Expression * __Scalar
template <typename T, int N, typename LExpr>
inline __Expression<T, N, __Multiplies<T, LExpr, __Scalar<T>>> operator*(
    const __Expression<T, N, LExpr> &lhs, const T &rhs)
{
    return __Expression<T, N, __Multiplies<T, LExpr, __Scalar<T>>>(
        __Multiplies<T, LExpr, __Scalar<T>>(lhs.__expr, __Scalar<T>(rhs)));
}


// __Scalar * __Expression
template <typename T, int N, typename RExpr>
inline __Expression<T, N, __Multiplies<T, __Scalar<T>, RExpr>> operator*(
    const T &lhs, const __Expression<T, N, RExpr> &rhs)
{
    return __Expression<T, N, __Multiplies<T, __Scalar<T>, RExpr>>(
        __Multiplies<T, __Scalar<T>, RExpr>(__Scalar<T>(lhs), rhs.__expr));
}


// __Expression / __Expression
template <typename T, int N, typename LExpr, typename RExpr>
inline __Expression<T, N, __Divides<T, LExpr, RExpr>> operator/(
    const __Expression<T, N, LExpr> &lhs, const __Expression<T, N, RExpr> &rhs)
{
    return __Expression<T, N, __Divides<T, LExpr, RExpr>>(
        __Divides<T, LExpr, RExpr>(lhs.__expr, rhs.__expr));
}


// __Expression / __Scalar
template <typename T, int N, typename LExpr>
inline __Expression<T, N, __Divides<T, LExpr, __Scalar<T>>> operator/(
    const __Expression<T, N, LExpr> &lhs, const T &rhs)
{
    return __Expression<T, N, __Divides<T, LExpr, __Scalar<T>>>(
        __Divides<T, LExpr, __Scalar<T>>(lhs.__expr, __Scalar<T>(rhs)));
}


// __Scalar / __Expression
template <typename T, int N, typename RExpr>
inline __Expression<T, N, __Divides<T, __Scalar<T>, RExpr>> operator/(
    const T &lhs, const __Expression<T, N, RExpr> &rhs)
{
    return __Expression<T, N, __Divides<T, __Scalar<T>, RExpr>>(
        __Divides<T, __Scalar<T>, RExpr>(__Scalar<T>(lhs), rhs.__expr));
}


// __Expression % __Expression
template <typename T, int N, typename LExpr, typename RExpr>
inline __Expression<T, N, __Modulus<T, LExpr, RExpr>> operator%(
    const __Expression<T, N, LExpr> &lhs, const __Expression<T, N, RExpr> &rhs)
{
    return __Expression<T, N, __Modulus<T, LExpr, RExpr>>(
        __Modulus<T, LExpr, RExpr>(lhs.__expr, rhs.__expr));
}


// __Expression % __Scalar
template <typename T, int N, typename LExpr>
inline __Expression<T, N, __Modulus<T, LExpr, __Scalar<T>>> operator%(
    const __Expression<T, N, LExpr> &lhs, const T &rhs)
{
    return __Expression<T, N, __Modulus<T, LExpr, __Scalar<T>>>(
        __Modulus<T, LExpr, __Scalar<T>>(lhs.__expr, __Scalar<T>(rhs)));
}


// __Scalar % __Expression
template <typename T, int N, typename RExpr>
inline __Expression<T, N, __Modulus<T, __Scalar<T>, RExpr>> operator%(
    const T &lhs, const __Expression<T, N, RExpr> &rhs)
{
    return __Expression<T, N, __Modulus<T, __Scalar<T>, RExpr>>(
        __Modulus<T, __Scalar<T>, RExpr>(__Scalar<T>(lhs), rhs.__expr));
}


// 适用于__Expression的operator<<重载
template <typename T, int N, typename Expr>
ostream &operator<<(ostream &os, const __Expression<T, N, Expr> &expressionObj)
{
    os << '[';

    if (N)
    {
        os << expressionObj[0];

        for (int idx = 1; idx < N; idx++)
        {
            os << ", " << expressionObj[idx];
        }
    }

    os << ']';

    return os;
}


// 最终供用户使用的Array类
template <typename T, int N>
using Array = __Expression<T, N, __Array<T, N>>;


int main()
{
    // 默认构造函数
    Array<int, 3> sampleArrayA;  // [?, ?, ?]

    // Fill构造函数
    Array<int, 3> sampleArrayB(0);  // [0, 0, 0]

    // initializer_list构造函数
    Array<int, 3> sampleArrayC {1, 2, 3};  // [1, 2, 3]

    // 拷贝构造函数
    Array<int, 3> sampleArrayD(sampleArrayC);  // [1, 2, 3]

    // 四则运算
    Array<int, 3> arrayA {1, 2, 3}, arrayB {4, 5, 6}, resArray;

    // operator<<
    cout << arrayA << endl;    // [1, 2, 3]
    cout << arrayB << endl;    // [4, 5, 6]
    cout << resArray << endl;  // [?, ?, ?]

    // 惰性计算
    arrayA + arrayB;  // 什么都不做！
    cout << arrayA + arrayB << endl;  // [5, 7, 9]

    // operator+
    resArray = 2 + arrayA + arrayB + 2;
    cout << resArray << endl;  // [9, 11, 13]

    // operator-
    resArray = 2 - arrayA - arrayB - 2;
    cout << resArray << endl;  // [-5, -7, -9]

    // operator*
    resArray = 2 * arrayA * arrayB * 2;
    cout << resArray << endl;  // [16, 40, 72]

    // operator/
    resArray = 200 / arrayB / arrayA / 2;
    cout << resArray << endl;  // [25, 10, 5]

    // operator%
    resArray = 17 % arrayA % arrayB % 5;
    cout << resArray << endl;  // [0, 1, 2]
}
```

至此，表达式模板的实现也就全部完成了。

### 7.8 本章后记

表达式模板，作为一种服务于高性能计算场合的模板技术，被广泛应用于各种线性代数库中（如著名的Eigen库）。表达式模板的精彩之处在于：其充分利用了多级模板抽象所带来的更大的抽象能力，将表达式模板中产生的重重复杂类型完全隐藏于代码实现中，使得用户既能够像书写普通表达式那样进行公式的编码，亦能够享受到表达式模板所带来的极佳效率。模板在高性能计算领域的这一应用，既为模板技术再添精彩一笔，也为我们的故事画上了句号...

## 8 模板是黑魔法吗？——后记

模板，最早于上世纪90年代被引入至C++，此后的多年内，模板技术迅速发展，促使了大量与之相关的程序设计技术的出现与成熟，并直接导致了STL的出现。在模板出现的几年后，一份"通过报错信息计算质数"的程序代码彻底刷新了人们对于模板的认知，这直接导致了"模板元编程"这一概念的出现（本文作者原想以此份代码的解读作为后记前的最后一章，但这份代码已年代久远，已经不能在作者的GCC编译器上得到理想的效果了，故抱憾作罢）。C++98标准的确立，催生了包括《C++ Templates: The Complete Guide》、《Modern C++ Design》等大量优秀著作的产生。正如《Modern C++ Design》一书的中文译序中所言，这些书籍所讲述的技术，使得我们不再认为模板只是一位"戴上了新帽子"的旧朋友。阅读这些书籍，一定能让你对模板这一技术具有更深入，更全面的认知。

模板是黑魔法吗？类似的问题还有很多（例如：Python的元类是黑魔法吗？）。如果你是一个狂热的模板爱好者，你一定会回答：不！模板是很有用的工具！而如果你对模板不是很感兴趣，或仅仅是因为在学习模板的过程中感到吃力，也许你会对模板的实用性存疑。人对某一个学术领域，某一项技术的认知，必将随着学识、心态、技术本身的兴棧­因素的变化而不断的发生着变化。所以这个问题地答案，也就等着读者你自己去不断的回答了。

注：本文中的部分程序已完整实现于本文作者的Github上，列举如下：

1. 编译期分数：[https://github.com/yingyulou/Fraction](https://github.com/yingyulou/Fraction)
2. print函数：[https://github.com/yingyulou/pprint](https://github.com/yingyulou/pprint)
3. Tuple：[https://github.com/yingyulou/Tuple](https://github.com/yingyulou/Tuple)
4. 表达式模板：[https://github.com/yingyulou/ExprTmpl](https://github.com/yingyulou/ExprTmpl)

<p align=right>樱雨楼</p>
<p align=right>2020.7于苏州</p>
