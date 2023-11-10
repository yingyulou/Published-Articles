# 编译器实现之旅——第十六章 全局变量、main函数、代码装载与链接器

在上一章的旅程中，我们已经实现了函数调用的代码生成器分派函数，但在上一章的末尾，我们留下了三个问题：

1. 我们需要为全局变量压栈
2. main函数需要在程序启动时被自动调用
3. 我们需要实现一个链接器，以将所有的CALL伪指令转变为一条真正的CALL指令

所以，在这一章的旅程中，我们就将解决这三个遗留问题，为代码生成器的漫长旅途画上圆满的句号。

## 1. 全局变量

我们要解决的第一个问题是为全局变量压栈。首先，让我们来看看栈内存结构图中，与全局变量相关的部分：

```
+-------+-----+-----+-----+-----+-----+-----+  ...
| 索引值 |  0  |  1  |  2  |  3  |  4  |  5  |  ...
+-------+-----+-----+-----+-----+-----+-----+  ...
|   值  |  ?  |  2  |  ?  |  ?  |  ?  |  ?  |  ...
+-------+-----+-----+-----+-----+-----+-----+  ...
           ^     ^     ^     ^     ^     ^
           |     |     |     |     |     |
           a     b    b[0]  b[1]  b[2]   c
```

事实上，全局变量压栈的实现思路和上一章中局部变量压栈的实现思路是基本一致的，但全局变量压栈的实现要比局部变量压栈的实现简单得多，这主要归功于以下几点：

1. 在符号表中，\_\_GLOBAL\_\_键所存储的信息就是所有全局变量的信息，不需要进行类似于"将形参与局部变量分离"这样的操作
2. 全局变量也不需要进行类似于"倒序压栈"这样的操作，符号表中的变量编号就是栈中这个变量的索引值
3. 全局变量中的数组的第一个元素在栈中的索引值是编译期已知的：一定是符号表中的变量编号加1，并不需要借助相关的计算指令

有了上述结论作为铺垫，就让我们来看看全局变量压栈的实现吧。请看：

``` Cpp
vector<pair<__Instruction, string>> __CodeGenerator::__generateGlobalVariableCode() const
{
    vector<pair<__Instruction, string>> codeList;

    for (auto &[_, infoPair]: __symbolTable.at("__GLOBAL__"))
    {
        // Array
        if (infoPair.second)
        {
            // Calc the array start address (variable number + 1)
            codeList.emplace_back(__Instruction::__LDC, to_string(infoPair.first + 1));
        }

        // Push the array start address
        // (Or only a meaningless int for global scalar memeory)
        codeList.emplace_back(__Instruction::__PUSH, "");

        // Push array content (by array size times)
        for (int _ = 0; _ < infoPair.second; _++)
        {
            codeList.emplace_back(__Instruction::__PUSH, "");
        }
    }

    return codeList;
}
```

上述代码中，我们遍历符号表中\_\_GLOBAL\_\_键所对应的信息；如果当前变量的数组长度为0，则我们直接生成一条PUSH指令即可；否则，如果当前变量的数组长度不为0，则我们就将"符号表中的变量编号 + 1"装载入AX中，再执行PUSH，以将数组的第一个元素在栈中的索引值压栈；并继续压栈数组长度次。

## 2. main函数

本节中，我们将要为main函数的自动调用做准备。显然，main函数也是一个函数，所以调用main函数的思路与调用普通函数的思路是基本一致的，但调用main函数的实现要比调用普通函数的实现简单的多，这主要归功于以下几点：

1. main函数一定没有实参，故不需要进行实参压栈；此外，也就不需要"将形参与局部变量分离"这样的操作了
2. 调用main函数后，虚拟机将直接退出，故不需要进行退栈

也就是说，调用main函数的实现完全就是调用普通函数的实现的删减版，我们只需要将调用普通函数的实现删减至以下这几个操作即可：

1. 将局部变量倒序压栈
2. 追加一条"CALL main"伪指令

将\_\_generateCallCode函数的实现照搬过来，然后按照上文讨论的那样进行删减，我们就得到了调用main函数的实现。请看：

``` Cpp
vector<pair<__Instruction, string>> __CodeGenerator::__generateMainPrepareCode() const
{
    /*
        The "main" function is a special function, so the following code is
        similar with the function: __generateCallCode
    */

    vector<pair<__Instruction, string>> codeList;

    vector<pair<string, pair<int, int>>> pairList(__symbolTable.at("main").size());

    // ..., Local2, Local1, Local0
    for (auto &mapPair: __symbolTable.at("main"))
    {
        pairList[pairList.size() - mapPair.second.first - 1] = mapPair;
    }

    // The "main" function has definitely no params
    for (auto &[_, infoPair]: pairList)
    {
        if (infoPair.second)
        {
            for (int _ = 0; _ < infoPair.second; _++)
            {
                codeList.emplace_back(__Instruction::__PUSH, "");
            }

            codeList.emplace_back(__Instruction::__ADDR, to_string(infoPair.second));
            codeList.emplace_back(__Instruction::__PUSH, "");
        }
        else
        {
            codeList.emplace_back(__Instruction::__PUSH, "");
        }
    }

    // Call the "main" function automatically
    codeList.emplace_back(__Instruction::__CALL, "main");

    return codeList;
}
```

将我们刚刚实现的\_\_generateMainPrepareCode函数与上一节实现的\_\_generateGlobalVariableCode函数合并在一起，我们就得到了一个用于生成"全局代码"的函数：

``` Cpp
vector<pair<__Instruction, string>> __CodeGenerator::__generateGlobalCode() const
{
    auto codeList = __generateGlobalVariableCode();

    auto mainPrepareCodeList = __generateMainPrepareCode();

    codeList.insert(codeList.end(), mainPrepareCodeList.begin(), mainPrepareCodeList.end());

    return codeList;
}
```

main函数确实调用起来了，但是细心的读者可能已经发现了：上文中"调用main函数后，虚拟机将直接退出"从何而来？联想到虚拟机的实现中，我们也并没有讨论任何和"main函数"有关的话题呀。也许你已经猜到接下来的故事了：如果我们将main函数生成的代码强行排布在代码生成器生成的代码列表的最后一部分，那么当虚拟机执行完最后一条main函数的代码后，其就会自动退出了。没错，我们正要这么做。请接着往下看。

## 3. 代码装载

不难发现，虽然还有部分遗留问题没有解决，但我们所有的代码生成器分派函数，以及用于生成"全局代码"的函数均已实现，也就是说，我们已经可以为抽象语法树中的每个函数声明节点，以及一个虚拟的"\_\_GLOBAL\_\_"节点生成代码了。在生成代码的同时我们不能忘记：在每个函数的末尾，也就是这个函数执行完毕的时候，我们都需要追加一条RET指令，以使得IP重新回到调用点，唯一的例外是main函数，其不需要这条指令。

首先，我们可以将每个函数的函数名及其生成的代码组织为一个哈希表，为后续操作做准备。请看：

``` Cpp
unordered_map<string, vector<pair<__Instruction, string>>> __CodeGenerator::__createCodeMap() const
{
    unordered_map<string, vector<pair<__Instruction, string>>> codeMap
    {
        {"__GLOBAL__", __generateGlobalCode()},
    };

    /*
        __TokenType::__Program
            |
            |---- __Decl
            |
            |---- [__Decl]
            .
            .
            .
    */
    for (auto declPtr: __root->__subList)
    {
        /*
            __VarDecl | __FuncDecl
        */
        if (declPtr->__tokenType == __TokenType::__FuncDecl)
        {
            /*
                __TokenType::__FuncDecl
                    |
                    |---- __Type
                    |
                    |---- __TokenType::__Id
                    |
                    |---- __ParamList | nullptr
                    |
                    |---- __LocalDecl
                    |
                    |---- __StmtList
            */
            auto curFuncName = declPtr->__subList[1]->__tokenStr;

            auto codeList = __generateStmtListCode(declPtr->__subList[4], curFuncName);

            if (curFuncName != "main")
            {
                /*
                    The instruction "RET" perform multiple actions:

                    1. IP = SS.POP()

                        Now the SS is like:

                        ... Local5 Local4 Local3 Param2 Param1 Param0 OldBP

                    2. BP = SS.POP()

                        Now the SS is like:

                        ... Local5 Local4 Local3 Param2 Param1 Param0

                    So we still need several "POP" to pop all variables.
                    (See the function: __generateCallCode)
                */
                codeList.emplace_back(__Instruction::__RET, "");
            }

            codeMap[curFuncName] = codeList;
        }
    }

    return codeMap;
}
```

得到这个代码哈希表后，我们就得到了一个重要信息：每个函数所生成的代码分别有多少条。此时，如果我们还知道每个函数在CS中的排列顺序，我们就可以计算出每个函数的第一条指令在CS中的索引值了。而有了这个索引值，我们也就能够将所有的CALL伪指令转变为真正的CALL指令了。事实上，当我们生成哈希表时，每个函数在CS中的排列顺序就已经确定了。但我们不能忘记两个特例：

1. 显然，"全局代码"应该出现在CS的开头
2. main函数必须排在最后。正如上文中已经讨论过的那样：这么做的目的是为了让虚拟机在执行完main函数的最后一条指令后自动退出

也就是说，CS中指令的排列应该如下所示：

```
"全局代码"的第一条指令
"全局代码"的第二条指令
"全局代码"的第三条指令
...
"全局代码"的最后一条指令（一定是"CALL main"）
函数A的第一条指令
函数A的第二条指令
函数A的第三条指令
...
函数A的最后一条指令
函数B的第一条指令
函数B的第二条指令
函数B的第三条指令
...
函数B的最后一条指令
...
main函数的第一条指令
main函数的第二条指令
main函数的第三条指令
...
main函数的最后一条指令
```

由此，我们可以在满足上述两个特例的前提下，遍历代码哈希表，并同时做两件事：

1. 将代码哈希表中的各个代码列表合并到一个代码列表中
2. 计算每个函数的第一条指令在CS中的索引值

请看：

``` Cpp
pair<vector<pair<__Instruction, string>>, unordered_map<string, int>> __CodeGenerator::__mergeCodeMap(
    const unordered_map<string, vector<pair<__Instruction, string>>> &codeMap)
{
    vector<pair<__Instruction, string>> codeList;

    // funcJmpMap: Function name => Function start IP
    unordered_map<string, int> funcJmpMap;

    // Global code must be the first part
    int jmpNum = codeMap.at("__GLOBAL__").size();

    codeList.insert(codeList.end(), codeMap.at("__GLOBAL__").begin(), codeMap.at("__GLOBAL__").end());

    // Other functions
    for (auto &[funcName, subCodeList]: codeMap)
    {
        if (funcName != "__GLOBAL__" && funcName != "main")
        {
            codeList.insert(codeList.end(), subCodeList.begin(), subCodeList.end());

            funcJmpMap[funcName] = jmpNum;

            jmpNum += subCodeList.size();
        }
    }

    // The "main" function must be the last function
    codeList.insert(codeList.end(), codeMap.at("main").begin(), codeMap.at("main").end());

    funcJmpMap["main"] = jmpNum;

    return {codeList, funcJmpMap};
}
```

正如上文中所讨论的那样，\_\_mergeCodeMap以一种特殊的顺序遍历codeMap，在遍历的过程中，其同时做了两件事：

1. 将当前遍历到的子代码列表合并至codeList
2. 通过累加jmpNum的方式，计算每个函数的第一条指令在CS中的索引值，并以函数名作为键，将此索引值存放在funcJmpMap中

至此，我们就完成了代码装载。

## 4. 链接器

接下来，我们开始实现链接器。链接器的功能很简单：遍历所有生成的指令，找到并转变其中的每一条CALL伪指令至一条真正的CALL指令。说到这里，也许你已经十分明确了：我们已经有能力确定任意一条指令在CS中的索引值，这当然就包括所有的CALL指令；我们也已经得到了每个函数的第一条指令在CS中的索引值；现在，我们只需要用被调用的函数（即CALL伪指令的参数）的第一条指令在CS中的索引值，减去当前CALL伪指令在CS中的索引值，就是真正的CALL指令需要跳转的位置了。请看：

``` Cpp
void __CodeGenerator::__translateCall(vector<pair<__Instruction, string>> &codeList, const unordered_map<string, int> &funcJmpMap)
{
    // A virtual "IP"
    for (int IP = 0; IP < (int)codeList.size(); IP++)
    {
        if (codeList[IP].first == __Instruction::__CALL)
        {
            codeList[IP].second = to_string(funcJmpMap.at(codeList[IP].second) - IP);
        }
    }
}
```

上述代码中，我们创建了一个虚拟的IP，以跟踪每一条指令在CS中的索引值。在遍历codeList的过程中，如果我们发现当前指令是一条CALL伪指令，我们就使用CALL伪指令后接的函数名，在funcJmpMap中查到这个函数的第一条指令在CS中的索引值，并将其与IP相减，就得到了真正的CALL指令需要的参数。

## 5. 将它们合并在一起

经历了漫长的旅途，我们终于为代码生成器的最终实现铺平了一切道路。现在，我们要做的便是将它们合并在一起。请看：

``` Cpp
vector<pair<__Instruction, string>> __CodeGenerator::__generateCode() const
{
    auto codeMap = __createCodeMap();

    auto [codeList, funcJmpMap] = __mergeCodeMap(codeMap);

    __translateCall(codeList, funcJmpMap);

    return codeList;
}
```

至此，代码生成器，乃至整个CMM编译器的实现，就都已经全部完成了。而我们的旅程，也即将到达终点...

请看下一章：《编译器实现之旅——第十七章 终章》。



上一章：[编译器实现之旅——第十五章 函数调用的代码生成器分派函数的实现](编译器实现之旅——第十五章%20函数调用的代码生成器分派函数的实现.md)

下一章：[编译器实现之旅——第十七章 终章](编译器实现之旅——第十七章%20终章.md)
