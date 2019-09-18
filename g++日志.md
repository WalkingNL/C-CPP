## g++ 的使用记录

下面以list.cpp、list.h为例，进行展开。

### 静态库
###### 假设 list.cpp 与 list.h在同一目录下
    $ g++ -std=c++11 -c list.cpp
  * -c 表示编译源文件，在这里就是编译list.cpp; 
  * -std=c++11 按照c++11的标准对list.cpp进行编译。
  * 如果编译成功，默认，即不显示的指定输出文件，会生成 list.o，可以看出，使用.o的后缀替换原文件.cpp的后缀。
  
如果指定输出文件，添加-o list_x.o，如下

    $ g++ -std=c++11 -c list.cpp -o list_x.o
那么这样的会，就会输出 list_x.o的文件
