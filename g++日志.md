## g++ 的使用记录

下面以list.cpp、list.h为例，进行展开。

### 静态库
###### 基本储备
    $ g++ -std=c++11 -c list.cpp  // (1)
  * -c 表示编译源文件，在这里就是编译list.cpp; 
  * -std=c++11 按照c++11的标准对list.cpp进行编译。
  * 如果编译成功，默认，即不显示的指定输出文件，会生成 list.o，可以看出，使用.o的后缀替换原文件.cpp的后缀。
  
如果指定输出文件，添加-o list_x.o，如下

    $ g++ -std=c++11 -c list.cpp -o list_x.o  // (2)
那么这样的话，就会输出 list_x.o的文件。这个后缀为`.o`的文件能干什么呢？或者说使用`-o`本质用意是什么呢？通过执行man g++，能够看到下面的解释
> Place output in file file. This applies to whatever sort of output is being produced, whether it be an executable file, an object file, an assembler file or preprocessed C code. If -o is not specified, the default is to put an executable file in a.out, the object file for source.suffix in source.o, its assembler file in source.s, a precompiled header file in source.suffix.gch, and all preprocessed C source on standard output.

简单的翻译一下就是
> 放置输出文件在文件file中。这对于产生的任何类型的输出文件都是适用的，不管是可执行文件、目标文件、汇编文件还是预处理的C代码。如果没有指定-o，缺省输出一个a.out的可执行文件、source.o的目标文件、source.s的汇编文件、source.suffix.gch的预编译头文件，以及标准输出上的全部被预处理过的C源码。

如果还不明白的话，可以做个尝试。list.cpp中并没有main()函数，因此，不能生成一个可执行文件。比如用下面的命令：
    
    $ g++ -std=c++11 list.cpp  // (3)
会产生下面的输出结果，提示找不到入口函数。
![](https://github.com/WalkingNL/Pics/blob/master/gcc.jpg)

在list.cpp中写上main()函数之后，把上面的命令再执行一次，就会发现，在当前目录(默认输出路径)中，出现a.out，正如上面解释的，他是一个可执行文件。

###### 生成静态库
为了生成一个静态库，把list.cpp中的main()函数去掉。使用命令

    $ g++ -std=c++11 -c list.cpp -o lib_list.o  // (4)
显式的指定生成一个lib_list.o的文件。

通用的命令格式

    ar rcs 指定路径/static_lib.a 指定路径/obj1.o 指定路径/obj2.o 指定路径/obj3.o ......
一个静态库就是一系列目标文件的集合，这些目标文件被拷贝至一个后缀为`.a`的单个文件中。在我们的示例中，因为只有一个lib_list.o的文件，且都在一个目录中，故命令如下。这样就生成了lib_list.a的静态库。如何使用这个静态库呢？

    $ ar rcs lib_list.a lib_list.o  // (5)

###### 使用静态库
现在，在另一个目录中，新建了一个main.cpp文件，里面加入了main()函数，需要调用上面list.cpp中的方法。首先编译main.cpp，使用下面的命令

    $ g++ -c main.cpp -o main.o  // (6)
然后生成可执行文件，命令见下

    $ g++ -o main main.o /test/lib_list.a  // (7)
这样，生成可执行文件`main`。


### 动态库
