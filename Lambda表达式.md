
在C++11及之后的C++版本中，lambda表达式————很多时候也称称*lambda*————是定义匿名函数对象的一种便捷的方法。

lambda表达式是定义匿名函数对象(或闭包)的一种便捷方法

被调用或被作为参数传递到一个函数


lambda表达式（通常称为lambda）是一种在调用或作为函数参数传递的位置处定义匿名函数对象（闭包）的便捷方法


a lambda expression is a convenient way of defining an asynoymous object(a closure) right at the location where it is invoked or passed as an argument to a function


lambda表达式一种在函数调用处或适当的位置处定义匿名函数对象(闭包)的便捷方法


### lambda表达式组成结构
ISO C++标准给出了一种简单的lambda表达式，它被作为第三个参数传递给std::sort()函数：

    #include <algorithm>
    #include <cmath>

    void abssort(float* x, unsigned n) {
        std::sort(x, x + n,
            // Lambda expression begins
            [](float a, float b) {
                return (std::abs(a) < std::abs(b));
            } // end of lambda expression
        );
    }
下图展示了一个lambda表达式的组成结构

![](https://github.com/WalkingNL/Pics/blob/master/86C5DD1F-9551-44B4-BA5D-127AC9EF53D8.png)
1. 捕捉列表(也称lambda表达式的引出符)
2. 参数列表(可选，当不需要传递参数时，可以省略)
3. mutable修饰符(可选，默认情况下，lambda是一个const函数，mutable修饰符也可省略。如果要使用它，参数列表也同样不能省略掉，即使参数为空)
4. 异常规范(可选)
5. 追踪返回类型(可选，当不需要返回时，也可以省略掉连同`->`富豪榜一起)
6. lambda函数体

#### 捕捉列表
在C++14里，lambda函数体中允许引入新的的变量，并且也可以访问捕捉来自其周边域中的变量。一个lambda函数以捕捉列表作为开始(标准语法是lambda引入符)，在捕捉列表里面指定要捕捉的变量，以及变量应该以值还是引用的方式被捕捉。若变量前带有&符号，则表示以引用的方式被访问，否则，表示以值的方式被访问。


