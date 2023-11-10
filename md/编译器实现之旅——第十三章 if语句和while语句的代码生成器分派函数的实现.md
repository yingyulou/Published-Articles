# 编译器实现之旅——第十三章 if语句和while语句的代码生成器分派函数的实现

在上一章的旅程中，我们已经实现了表达式类代码生成器分派函数，而在这一章的旅程中，我们将要实现if语句和while语句的代码生成器分派函数。if语句和while语句是两种典型的带有跳转指令的语句。观察CMM指令集的实现不难发现，跳转指令，实际上就是通过强行修改IP的值，使得虚拟机下一次看到的CS[IP]并不是当前CS[IP]的下一条语句。所以，跳转指令的摆放位置及其参数，是我们在实现if语句和while语句的分派函数的过程中，需要思考的问题。那么接下来，就让我们先从if语句开始吧。

## 1. IfStmt节点的分派函数的实现

IfStmt节点，即if语句节点，其可能含有两个或三个子节点：第一子节点代表了if语句的条件表达式部分；而第二子节点代表了如果满足条件，需要执行的部分；第三子节点则代表了else部分。不难想到：如果一个IfStmt节点只含有两个子节点，即不含有else部分，那么其代码模型如下所示：

```
if 第一子节点生成的代码（其结果最终存放于AX中）

    JZ END

    第二子节点生成的代码

END: ...
```

现在的问题是：我们的CMM指令集根本就没有"JZ END"这一说，JZ只能后接一个数字，这怎么办呢？很简单，我们将"END"转换为第二子节点生成的代码的条数即可。例如：第二子节点一共生成了5条指令，那么，我们就需要生成一条"JZ 6"指令，请注意，JZ后面的数字是代码条数5再加上1，如果不加1的话，就会跳转到第二子节点生成的最后一条代码上，这样的边界问题是一定要注意的。

而如果一个IfStmt节点含有三个子节点呢？此时的代码模型就变为了这样：

```
if 第一子节点生成的代码（其结果最终存放于AX中）

    JZ ELSE

    第二子节点生成的代码

    JMP END

else

    ELSE: 第三子节点生成的代码

END: ...
```

此时的情况就要稍复杂一些，"JMP END"需要被转换为第三子节点生成的代码的条数加1；而由于"JMP END"的存在，"JZ ELSE"就需要被转换为第二子节点生成的代码的条数加1，再加1，以同时越过第二子节点生成的所有代码以及一条多出来的"JMP END"指令。

有了上面代码模型的铺垫，我们就不难得到IfStmt节点的分派函数的实现了。请看：

``` Cpp
vector<pair<__Instruction, string>> __CodeGenerator::__generateIfStmtCode(__AST *root, const string &curFuncName) const
{
    /*
        __TokenType::__IfStmt
            |
            |---- __Expr
            |
            |---- __StmtList
            |
            |---- [__StmtList]
    */

    auto codeList   = __generateExprCode(root->__subList[0], curFuncName);
    auto ifCodeList = __generateStmtListCode(root->__subList[1], curFuncName);

    if (root->__subList.size() == 2)
    {
        /*
            if ...

                JZ END

                ...

            END: ...
        */
        codeList.emplace_back(__Instruction::__JZ, to_string(ifCodeList.size() + 1));
        codeList.insert(codeList.end(), ifCodeList.begin(), ifCodeList.end());
    }
    else
    {
        /*
            if ...

                JZ ELSE

                ...

                JMP END

            else

                ELSE: ...

            END: ...
        */
        auto elseCodeList = __generateStmtListCode(root->__subList[2], curFuncName);

        ifCodeList.emplace_back(__Instruction::__JMP, to_string(elseCodeList.size() + 1));

        codeList.emplace_back(__Instruction::__JZ, to_string(ifCodeList.size() + 1));
        codeList.insert(codeList.end(), ifCodeList.begin(), ifCodeList.end());
        codeList.insert(codeList.end(), elseCodeList.begin(), elseCodeList.end());
    }

    return codeList;
}
```

## 2. WhileStmt节点的分派函数的实现

WhileStmt节点，即while语句节点，其实现与上文中的if语句是类似的。稍有不同的是，while语句中需要用到一次JMP的参数为负数的向前跳转，但其原理是不变的；并且，while语句也没有子节点数量的差异，一定具有两个子节点。接下来，就让我们来看看while语句的代码模型吧：

```
START: while 第一子节点生成的代码（其结果最终存放于AX中）

    JZ END

    第二子节点生成的代码

    JMP START

END: ...
```

和我们讨论if语句时一样，我们现在需要思考的是：如何转换这两条"JZ END"和"JMP START"指令？首先看"JZ END"指令，和上文中那个"加1，再加1"的"JZ ELSE"指令类似，由于"JMP START"指令的存在，"JZ END"指令的"END"需要被转换为第二子节点生成的代码的条数加1，再加1；而"JMP START"指令现在对我们来说也不是一个难点了，其需要被转换为**负的**第二子节点生成的代码的条数再减1（请注意，这里并不是减1，再减1）。

有了上面代码模型的铺垫，我们就不难得到WhileStmt节点的分派函数的实现了。请看：

``` Cpp
vector<pair<__Instruction, string>> __CodeGenerator::__generateWhileStmtCode(__AST *root, const string &curFuncName) const
{
    /*
        __TokenType::__WhileStmt
            |
            |---- __Expr
            |
            |---- __StmtList
    */

    auto codeList     = __generateExprCode(root->__subList[0], curFuncName);
    auto stmtCodeList = __generateStmtListCode(root->__subList[1], curFuncName);

    /*
        START: while ...

            JZ END

            ...

            JMP START

        END: ...
    */
    codeList.emplace_back(__Instruction::__JZ, to_string(stmtCodeList.size() + 2));
    codeList.insert(codeList.end(), stmtCodeList.begin(), stmtCodeList.end());
    codeList.emplace_back(__Instruction::__JMP, "-" + to_string(codeList.size()));

    return codeList;
}
```

至此，if语句和while语句的代码生成器分派函数的实现就全部完成了。接下来，我们将要实现的是一类看似平淡无奇，实则暗藏玄机的操作。请看下一章：《变量存取的代码生成器分派函数的实现》。



上一章：[编译器实现之旅——第十二章 表达式类代码生成器分派函数的实现](编译器实现之旅——第十二章%20表达式类代码生成器分派函数的实现.md)

下一章：[编译器实现之旅——第十四章 变量存取的代码生成器分派函数的实现](编译器实现之旅——第十四章%20变量存取的代码生成器分派函数的实现.md)
