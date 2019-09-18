## gcc/g++ 的使用记录

以list.cpp、list.h为例，进行展开。

### 静态库
####### 假设 list.cpp 与 list.h在同一目录下
    $ g++ -std=c++11 -c list.cpp
    // -c 表示编译源文件，在这里就是编译list.cpp 
