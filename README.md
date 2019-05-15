# C MINUS 解析器

* 语言：C++
* 工具：DEV-C++

根据实验课程要求，仅实现了C Minus 的词法分析器与语法分析器。
两个功能由于相关性较大，且个人没有使用DEV-C++的工程来开发，所以两个功能放在了一个类中实现。

## C MINUS 词法分析器
* 已将C minus 的关键字写入到代码中，读入一个C minus 语法的代码后，即可生成相应的符号表、非终结符。
* 利用循环语句+IF语句实现不同词法单元的DFA。

## C MINUS 语法分析器
* 已将C minus 的非终结符与终结符写入到了代码中。
* 语法分析器基于LL1文法，所以需要读入C Minus 的LL1文法规则。
* 读入一个C minus语法的代码后，即可生成相应的语法分析树，然后可执行输出功能。
* 使用非递归的预测分析法。

## 可视化结果
* 使用python的Graphviz库可视化语法分析树。文件为vis_tree.py
* 使用python的Anytree库建立了语法分析树并还原读入的去除换行与注释后的代码内容。文件为return_code.py
两个文件都位于visualization目录下（由于Anytree库也具有可视化功能，虽然在此没有使用，但仍放入当前目录下）。
