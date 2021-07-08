# 编译器实现之旅——第十六章 代码装载、链接器、全局变量与main函数

在上一章的旅程中，我们已经实现了函数调用的代码生成器分派函数，但在上一章的末尾，我们留下了三个问题：

1. 我们需要实现一个链接器，以将所有的CALL伪指令转变为一条真正的JMP指令
2. 我们需要为全局变量压栈
3. main函数需要在程序启动时被自动调用

所以，在这一章的旅程中，我们就将解决这三个遗留问题，为代码生成器的漫长旅途画上圆满的句号。

## 1. 代码装载

不难发现，虽然还有部分遗留问题没有解决，但我们所有的代码生成器分派函数均已实现，也就是说，我们已经可以为抽象语法树中的每个函数声明节点生成代码了。在生成代码的同时我们不能忘记：在每个函数的末尾，也就是这个函数执行完毕的时候，我们都需要追加一条POPIP指令，以使得IP重新回到调用点；唯一的例外是main函数，其需要追加的是STOP指令而不是POPIP指令，以使得main函数在执行完毕的时候终止虚拟机；此外，在生成每个函数的代码之前，我们也不能忘记及时更新\_\_nowFuncName变量，以供相关的代码生成器分派函数使用。

我们可以将每个函数的函数名及其生成的代码组织为一个哈希表，这样就完成了代码装载。请看：

``` Cpp
unordered_map<string, vector<string>> CodeGenerator::__createCodeMap()
{
    unordered_map<string, vector<string>> codeMap;

    /*
        TOKEN_TYPE::DECL_LIST
            |
            |---- __Decl
            |
            |---- [__Decl]
            .
            .
            .
    */
    for (auto declPtr: __root->subList)
    {
        /*
            __VarDecl | __FuncDecl
        */
        if (declPtr->tokenType == TOKEN_TYPE::FUNC_DECL)
        {
            /*
                TOKEN_TYPE::FUNC_DECL
                    |
                    |---- __Type
                    |
                    |---- TOKEN_TYPE::ID
                    |
                    |---- __Params
                    |
                    |---- __CompoundStmt
            */
            __nowFuncName = declPtr->subList[1]->tokenStr;

            /*
                TOKEN_TYPE::COMPOUND_STMT
                    |
                    |---- __LocalDecl
                    |
                    |---- __StmtList
            */
            vector<string> codeList = __generateStmtListCode(declPtr->subList[3]->subList[1]);

            codeList.push_back(__nowFuncName == "main" ? "STOP" : "POPIP");

            codeMap[__nowFuncName] = codeList;
        }
    }

    return codeMap;
}
```

## 2. 链接器

代码装载完成后，我们就得到了一个重要信息：每个函数所生成的代码分别有多少条。此时，如果我们还知道每个函数在CS中的排列顺序，我们就可以最终计算得到每个函数的第一条指令在CS中的索引值了。而有了每个函数的第一条指令在CS中的索引值，我们也就能够将所有的CALL伪指令转变为JMP指令了。事实上，当我们生成哈希表时，每个函数在CS中的排列顺序就已经确定了，唯独不同的是：main函数必须排在第一个，也就是说，CS中指令的排列应该如下所示：

```
    main函数的第一条指令
    main函数的第二条指令
    main函数的第三条指令
    ...
    main函数的最后一条指令
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
```

由此，我们就可以很轻松的计算每个函数的第一条指令在CS中的索引值了。请看：

``` Cpp
unordered_map<string, int> CodeGenerator::__createFuncJmpMap(
    const unordered_map<string, vector<string>> &codeMap) const
{
    // funcJmpMap: Function name => Function start IP
    unordered_map<string, int> funcJmpMap;

    // "main" function must be the first function
    int jmpNum = codeMap.at("main").size();

    // Other function
    for (auto &mapPair: codeMap)
    {
        if (mapPair.first != "main")
        {
            funcJmpMap[mapPair.first] = jmpNum;
            jmpNum += mapPair.second.size();
        }
    }

    return funcJmpMap;
}
```

\_\_createFuncJmpMap函数得到一个哈希表，其以函数名作为键，函数的第一条指令在CS中的索引值作为值。这个函数的实现也很简单：一边遍历每个函数生成的代码，一边累加jmpNum即可。

接下来，我们开始实现链接器。链接器的功能很简单：遍历所有生成的指令，找到并转变其中的每一条CALL指令至JMP指令。说到这里，也许你已经十分明确了：我们已经有能力确定任意一条指令在CS中的索引值，这当然就包括所有的CALL指令；我们也已经得到了每个函数的第一条指令在CS中的索引值，现在，我们只需要再得到每一条CALL指令在CS中的索引值，并将其与需要被调用的函数（即CALL指令的参数）的第一条指令在CS中的索引值相减，就是JMP指令需要跳转的位置了。请看：

``` Cpp
void CodeGenerator::__translateCall(unordered_map<string, vector<string>> &codeMap) const
{
    auto funcJmpMap = __createFuncJmpMap(codeMap);

    // A virtual "IP"
    int IP = 0;

    // "main" function must be the first function
    for (auto &codeStr: codeMap.at("main"))
    {
        if (codeStr.substr(0, 4) == "CALL")
        {
            // Function start IP - IP => Jump distance
            codeStr = "JMP " + to_string(funcJmpMap.at(codeStr.substr(5)) - IP);
        }

        IP++;
    }

    // Other function
    for (auto &mapPair: codeMap)
    {
        if (mapPair.first != "main")
        {
            for (auto &codeStr: mapPair.second)
            {
                if (codeStr.substr(0, 4) == "CALL")
                {
                    codeStr = "JMP " + to_string(funcJmpMap.at(codeStr.substr(5)) - IP);
                }

                IP++;
            }
        }
    }
}
```

上述代码中，我们创建了一个虚拟的IP，以跟踪每一条指令在CS中的索引值。然后，我们严格按照CS中指令的排列顺序遍历每条指令，并递增虚拟的IP。当发现当前指令是一条CALL伪指令时，我们使用CALL后接的函数名，在funcJmpMap哈希表中查到这个函数的第一条指令在CS中的索引值，并将其与当前的虚拟的IP值相减，就得到了JMP指令的参数。

## 3. 全局变量压栈

本节中，我们将要实现的是将所有全局变量进行压栈的代码生成器函数。首先，我们将栈内存结构图中，与全局变量相关的部分展示如下：

```
+--------+-----+-----+-----+-----+-----+-----+  ...
| 索引值 |  0  |  1  |  2  |  3  |  4  |  5  |  ...
+--------+-----+-----+-----+-----+-----+-----+  ...
|   值   |  ?  |  2  |  ?  |  ?  |  ?  |  ?  |  ...
+--------+-----+-----+-----+-----+-----+-----+  ...
            ^     ^     ^     ^     ^     ^
            |     |     |     |     |     |
            a     b    b[0]  b[1]  b[2]   c
```

事实上，全局变量压栈的实现思路和上一章中局部变量压栈的实现思路是基本一致的，但全局变量压栈的实现要比局部变量压栈的实现简单得多，这主要归功于以下几点：

1. 在符号表中，__GLOBAL__键所存储的信息就是所有全局变量的信息，不需要进行类似于“将形参与局部变量分离”这样的操作
2. 全局变量也不需要进行类似于“倒序压栈”这样的操作，符号表中的变量编号就是栈中这个变量的索引值
3. 全局变量中的数组的第一个元素在栈中的索引值是编译期已知的：一定是符号表中的变量编号加1，并不需要借助相关的计算指令

有了上述结论作为铺垫，就让我们来看看全局变量压栈的代码生成器函数的实现吧。请看：

``` Cpp
vector<string> CodeGenerator::__generateGlobalVariableCode() const
{
    vector<string> codeList;

    for (auto &mapPair: __symbolTable.at("__GLOBAL__"))
    {
        // Array
        if (mapPair.second.second)
        {
            // Calc the array start address (Variable number + 1)
            codeList.push_back("LDC " + to_string(mapPair.second.first + 1));
        }

        // Push the array start address
        // (Or only a meaningless int for global scalar memeory)
        codeList.push_back("PUSH");

        // Push array content (By array size times)
        for (int _ = 0; _ < mapPair.second.second; _++)
        {
            codeList.push_back("PUSH");
        }
    }

    return codeList;
}
```

上述代码中，我们遍历符号表中\_\_GLOBAL\_\_键所对应的信息；如果当前变量的数组长度为0，则我们直接生成一条PUSH指令即可；否则，如果当前变量的数组长度不为0，则我们就将“符号表中的变量编号 + 1”装载入AX中，再执行PUSH，以将数组的第一个元素在栈中的索引值压栈；并继续压栈数组长度次。

## 4. main函数

本节中，我们将要实现的是main函数的自动调用。显然，main函数也是一个函数，所以调用main函数的实现思路与调用普通函数的实现思路是基本一致的，但调用main函数的实现要比调用普通函数的实现简单的多，这主要归功于以下几点：

1. main函数一定没有实参，故不需要进行实参压栈；此外，也就不需要“将形参与局部变量分离”这样的操作了
2. 调用main函数不需要进行IP寄存器准备和函数调用跳转，CS[0]就是main函数的第一条指令
3. 调用main函数后，虚拟机将直接执行STOP指令，故不需要进行退栈

也就是说，调用main函数的实现完全就是调用普通函数的实现的删减版，我们只需要将调用普通函数的实现删减至以下这几个操作即可：

1. 将局部变量倒序压栈
2. 追加PUSHBP和SAVSP指令

事实上，main函数也不需要执行PUSHBP指令，因为此时的BP一定是无意义的，但为了与其他函数的栈内存结构保持一致，我们不能省略这条指令。

将\_\_generateCallCode函数的实现照搬过来，然后按照上文讨论的那样进行删减，我们就得到了调用main函数的实现。请看：

``` Cpp
vector<string> CodeGenerator::__generateMainPrepareCode() const
{
    /*
        "main" function is a special function, so the following code is similar
        with the function: __generateCallCode
    */

    vector<string> codeList;

    vector<pair<string, pair<int, int>>> pairList(__symbolTable.at("main").size());

    // ..., Local2, Local1, Local0
    for (auto &mapPair: __symbolTable.at("main"))
    {
        pairList[pairList.size() - mapPair.second.first - 1] = mapPair;
    }

    // "main" function has definitely no params
    for (auto &mapPair: pairList)
    {
        if (mapPair.second.second)
        {
            for (int _ = 0; _ < mapPair.second.second; _++)
            {
                codeList.push_back("PUSH");
            }

            codeList.push_back("PUSHSP");
            codeList.push_back("LDC " + to_string(mapPair.second.second - 1));
            codeList.push_back("SUB");
            codeList.push_back("POP");
            codeList.push_back("PUSH");
        }
        else
        {
            codeList.push_back("PUSH");
        }
    }

    codeList.push_back("PUSHBP");
    codeList.push_back("SAVSP");

    return codeList;
}
```

## 5. 将它们合并在一起

经历了漫长的旅途，我们终于为代码生成器的最终实现铺平了一切道路。现在，我们要做的便是踏上这条通往实现的道路：我们需要将代码生成器的各个部分生成的代码按顺序进行合并，顺序如下：

1. 全局变量压栈的代码
2. 调用main函数的代码
3. main函数的函数体的代码
4. 其他函数的函数体的代码

按照这个顺序，我们就可以得到代码生成器最终部分的实现。请看：

``` Cpp
void CodeGenerator::GenerateCode()
{
    auto codeMap = __createCodeMap();

    __translateCall(codeMap);

    auto globalVariableCodeList = __generateGlobalVariableCode(),
        mainPrepareCodeList = __generateMainPrepareCode();

    __outputCode(codeMap, globalVariableCodeList, mainPrepareCodeList);
}


void CodeGenerator::__outputCode(
    const unordered_map<string, vector<string>> &codeMap,
    const vector<string> &globalVariableCodeList,
    const vector<string> &mainPrepareCodeList) const
{
    FILE *fo = fopen(__outputFilePath.c_str(), "w");

    for (auto &codeStr: globalVariableCodeList)
    {
        fprintf(fo, "%s\n", codeStr.c_str());
    }

    for (auto &codeStr: mainPrepareCodeList)
    {
        fprintf(fo, "%s\n", codeStr.c_str());
    }

    for (auto &codeStr: codeMap.at("main"))
    {
        fprintf(fo, "%s\n", codeStr.c_str());
    }

    for (auto &mapPair: codeMap)
    {
        if (mapPair.first != "main")
        {
            for (auto &codeStr: mapPair.second)
            {
                fprintf(fo, "%s\n", codeStr.c_str());
            }
        }
    }

    fclose(fo);
}
```

至此，代码生成器，乃至整个CMM编译器的实现，就都已经全部完成了。而我们的旅程，也即将到达终点...

请看下一章：《编译器实现之旅——第十七章 终章》。

<br>

上一章：[编译器实现之旅——第十五章 函数调用的代码生成器分派函数的实现](编译器实现之旅——第十五章%20函数调用的代码生成器分派函数的实现.md)

下一章：[编译器实现之旅——第十七章 终章](编译器实现之旅——第十七章%20终章.md)