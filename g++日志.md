## g++ 的使用记录

下面以list.cpp、list.h为例，进行展开。

### 静态库
##### 基本储备
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

##### 生成静态库
为了生成一个静态库，把list.cpp中的main()函数去掉。使用命令

    $ g++ -std=c++11 -c list.cpp -o lib_list.o  // (4)
显式的指定生成一个lib_list.o的文件。

通用的命令格式

    ar rcs 指定路径/static_lib.a 指定路径/obj1.o 指定路径/obj2.o 指定路径/obj3.o ......
一个静态库就是一系列目标文件的集合，这些目标文件被拷贝至一个后缀为`.a`的单个文件中。在我们的示例中，因为只有一个lib_list.o的文件，且都在一个目录中，故命令如下。这样就生成了lib_list.a的静态库。如何使用这个静态库呢？

    $ ar rcs lib_list.a lib_list.o  // (5)

##### 使用静态库
现在，在另一个目录中，新建了一个main.cpp文件，里面加入了main()函数，需要调用上面list.cpp中的方法。首先编译main.cpp，使用下面的命令

    $ g++ -c main.cpp -o main.o  // 命令(6)
然后生成可执行文件，命令见下

    $ g++ -o main main.o /test/lib_list.a  // 命令(7)
这样，生成可执行文件`main`。

### 动态库
##### 创建动态库
通用的命令格式

    g++ -shared -o 指定路径/static_lib.so 指定路径/obj1.o 指定路径/obj2.o 指定路径/obj3.o ......
* -shared 把目标文件链接成共享对象

执行下面的命令创建动态库。
    
    $ g++ -shared -o dyn_lib_list.so lib_list.o  // 命令(8)
执行结果，如下图所示。可以看到创建.so文件失败了！！！
![](https://github.com/WalkingNL/Pics/blob/master/gcc-shared.jpg)

哦，好吧，没有加-fPIC选项。那么加上之后，命令(8)就变成了，见以下

    $ g++ -shared -fPIC -o dyn_lib_list.so ../Utilities/lib_list.o  // 命令(9)
首先解释一下，-fPIC是告诉编译器生成位置无关的代码。只有位置无关的代码才可以被加载到地址空间的任意位置而不需要修改。那么执行这条命令的结果参照下图
![](https://github.com/WalkingNL/Pics/blob/master/g%2B%2B%20fPIC.jpg)

和上一幅图对比一下，相同的错误。也就是说，加了-fPIC选项之后，对于执行结果，没有任何影响。既然没有影响，意味着这里加-fPIC选项是多余的。其实分析这条命令本身，就会发现，加-fPIC从语义上也讲不通，因为我们的本意是链接.o文件为.so文件，并非编译，而-fPIC是用于编译命令的选项。那如何解决图中的问题呢？其实仔细看一下图中的提示，有这么一句*recompile with -fPIC*。综合考虑就不难想到是`.o`文件的生成时，没有加-fPIC选项。对于静态库而言，自然不需要加-fPIC，但对于动态库，不加就不行了。所以，得重新生成`.o`文件，使用下面的命令
    
    $ g++ -std=c++11 -fPIC -c list.cpp -o lib_list.o  // 命令(10)
然后再使用`命令(8)`或者`命令(9)`(上面分析了，命令(9)中的-fPIC选项不起作用)，动态库`.so`文件就可以生成了。

[注：CSDN中参考了一篇[博客](https://blog.csdn.net/seanwang_25/article/details/20702751)，这里并没有解释清楚]
##### 使用动态库
###### 第一种方法
类似命令(7)那样，你可以用如下的命令使用动态库

    # g++ -o main main.o ../test/dyn_lib_xist.so  // 命令(11)
    # ./main //执行main
但这种方法，并没有发挥出动态库的优势，因此，更多时候，使用第二种方法
###### 第二种方法
命令如下所示。但是这个命令会报错，见后面的图。

    # g++ -o main main.o -L/test/ -ldyn_lib_xist.so  // 命令(12)
  * -L选项，指定动态库的路径
  * -l选项，指定动态库
![](https://github.com/WalkingNL/Pics/blob/master/g%2B%2B-L.jpg)

出现这个问题的原因是因为对动态库的命名方式不符合系统的要求，你可参考这个[链接](https://blog.csdn.net/u012655611/article/details/82858092)。更改命名后，见 命令(13)

    # g++ -o main main.o -L/test/ -lxist  // 命令(13)
对比命令(12)，发现`-l`之后不一样了，通过这样的写法，可以在`-L`指定的目录下，结合`-l`后的`xist`，最终找到完整的动态库libxist.so。如果你还不熟悉，在这个地方可以多试一下，原则上，根据动态库的命名方式(lib+名称+.so)，在-l后，接`名称`就可以了。否则，依然会报错。

但到这里还没有完，因为执行完`命令(13)`之后，运行`./main`，会出现下图示的错误
![](https://github.com/WalkingNL/Pics/blob/master/dynlib_fina.jpg)

