## 右值引用的符号：&&

#### 语法
    type-id && cast-expression

#### Remarks
右值引用能够使你区分左值和右值。左值引用与右值引用在句法及语义上是相似的，但它们各自遵循一些不同的规则。更多关于左值和右值的信息，看[这里](https://docs.microsoft.com/en-us/cpp/cpp/lvalues-and-rvalues-visual-cpp?view=vs-2019)。关于左值引用的信息，看[这里](https://docs.microsoft.com/en-us/cpp/cpp/lvalue-reference-declarator-amp?view=vs-2019)。

下面的几个部分会分别描述右值引用是如何支持*移动语义*及*完美转发*的实现的。

#### 移动语义
右值引用可支持*移动语义*，这大大提升了应用程序性能。因为*移动语义*实现了资源(例如分配的内存资源)在对象间的转移，这对于在程序中，任何地方出现的临时对象，无代价的转移其资源，大有罢意。

对于移动语义的实现，通常情况下，你需要在你的类中，提供一个移动构造函数，以及一个移动赋值运算符(**operator=**)。这样，资源为右值的拷贝及赋值操作可以自动的利用移动语义。但不像默认的拷贝构造函数那样，编译器不会提供默认的移动构造函数。如果要了解更多关于这方面的信息，参考[这里](https://docs.microsoft.com/en-us/cpp/cpp/move-constructors-and-move-assignment-operators-cpp?view=vs-2019)。

你也可以通过重载普通的函数及运算符以利用移动语义的优势。VS2010中引进移动语义到C++标准库中。如在`string`类中所实现的支持移动语义的相关操作。思考下面的示例，连接若干个字符串并打印结果：

    // string_concatenation.cpp
    // compile with: /EHsc
    #include <iostream>
    #include <string>
    using namespace std;

    int main()
    {
       string s = string("h") + "e" + "ll" + "o";
       cout << s << endl;
    }
在VS2010之前，每调用一次**operator**+，就得分配和返回一个临时对象(一个右值)。**operator**+因为不知道原字符串是左值还是右值，所以不能将一个串赋给另一个。如果原字符串是两个左值，他们可能在程序别的地方被引用，因此不能保证不被修改。这个时候，通过右值引用，**operator**+就能被改为获取右值，意味着在程序中，其它地方不可以引用它。故而，**operator**+现在就可以附加一个串到另一个串了。这极大的减少了内存分配的次数。更多关于`string`类的信息，看[这儿](https://docs.microsoft.com/en-us/cpp/standard-library/basic-string-class?view=vs-2019)

移动语义也能在编译器无法使用返回值优化(RVO或NRVO)时，提供帮助。在这种情况下，只要对象类型定义了移动构造函数，编译器就会调用它。有关NRVO的更多信息看[这里](https://docs.microsoft.com/en-us/previous-versions/ms364057(v=vs.80))。

为更好的理解移动语义，思考将元素插入到`vector`对象的例子。如果为`vector`对象分配的空间已经满了
，意味着要为`vector`对象重新分配内存，并且将所有元素拷贝到这个重新申请的空间中。一次插入操作就预示着要进行一次元素拷贝，调用拷贝构造函数将先前的元素变成新的元素，接着再释放掉先前的元素。很明显，这样反复拷贝的代价是巨大的，而移动语义摒弃了这一操作，转为直接移动对象。

`vector`示例展现了移动语义的优势所在，你完全可以自己写一个移动构造函数将数据从一个对象移动到另一个对象。


#### 完美转发
完美转发减少了对重载函数依赖，并且有助于规避转发问题的发生。当一个范型函数的参数为引用类型时，这个函数传递(或者说转发)这些参数到另一个函数时，*转发问题*就发生了。例如，如果这个范型函数拥有`const T&`类型的参数，那么这个被调函数不可能修改掉参数的值。而如果这个范型函数拥有`T&`类型的参数，那么该函数无法被一个传递右值类型参数的函数调用(如临时对象或者整型的字面值)。

一般来说，解决这样的问题，你需要分别提供针对参数类型为`T&`以及`const T&`，两种不同的重载函数。因此，随着参数数量的增多，对应重载函数的数量也会以指数量级的速度提升。而右值引用的出现，帮助你只需要写一个版本的函数，这个函数能够接收任意类型的参数，并且转发它们到另一个函数，就好像直接调用另一个函数一样。

思考下面的例子，声明四个结构体类型，`W`，`X`，`Y`，以及`Z`。每个类型的构造函数取**const**及**const**的不同组合作为参数。

    struct W
    {
       W(int&, int&) {}
    };

    struct X
    {
       X(const int&, int&) {}
    };

    struct Y
    {
       Y(int&, const int&) {}
    };

    struct Z
    {
       Z(const int&, const int&) {}
    };
假设你想要写一个范型函数，用其生成对象。以下示例展示了一种有此功能的函数：

    template <typename T, typename A1, typename A2>
    T* factory(A1& a1, A2& a2)
    {
       return new T(a1, a2);
    }    
以下示例展示了如何正确的调用这个`factory`函数：
    
    int a = 4, b = 5;
    W* pw = factory<W>(a, b);
然而，以下的函数却无法做到对`factory`的有效调用，因为`factory`取的是左值引用，其值可以被修改，但它却被右值类型的参数所调用：

    Z* pz = factory<Z>(2, 2);
一般来说，解决这个问题，创建与参数类型对应的不同版本的重载函数即可。而右值引用却帮助我们做到只需写一个版本的`factory`函数，如下面的例子所示：

    template <typename T, typename A1, typename A2>
    T* factory(A1&& a1, A2&& a2)
    {
       return new T(std::forward<A1>(a1), std::forward<A2>(a2));
    }    
这个例子使用右值引用作为`factory`函数的参数。[std::forward](https://docs.microsoft.com/en-us/cpp/standard-library/utility-functions?view=vs-2019#forward)转发这个`factory`函数的参数到对应模版类的构造函数。

以下的示例展示了在`main`函数中使用修订过的`factory`函数创建`W`，`X`，`Y`，以及`Z`类的实例。可以看到`factory`函数转发它的参数(左值或右值)到相应类的构造器中。

    int main()
    {
       int a = 4, b = 5;
       W* pw = factory<W>(a, b);
       X* px = factory<X>(2, b);
       Y* py = factory<Y>(a, 2);
       Z* pz = factory<Z>(2, 2);

       delete pw;
       delete px;
       delete py;
       delete pz;
    }

#### 右值引用的其他属性
**你可以重载一个函数获取一个右值引用以及一个左值引用**

通过重载一个函数，获取一个**const**类型的左值引用或一个右值引用，你可以编写出能够区分不可变的对象(左值)以及可变的临时值(右值)这样的代码。你可依传递一个对象到一个函数，除非这个对象被明确标记为**const**，否则这个函数获取一个右值引用作为参数。以下的示例展示了这个函数`f`，其作为重载函数出现，分别获取一个左值引用和一个右值引用，主函数`main`使用左值和右值分别调用函数`f`。

    // reference-overload.cpp
    // Compile with: /EHsc
    #include <iostream>
    using namespace std;

    // A class that contains a memory resource.
    class MemoryBlock
    {
       // TODO: Add resources for the class here.
    };

    void f(const MemoryBlock&)
    {
       cout << "In f(const MemoryBlock&). This version cannot modify the parameter." << endl;
    }

    void f(MemoryBlock&&)
    {
       cout << "In f(MemoryBlock&&). This version can modify the parameter." << endl;
    }

    int main()
    {
       MemoryBlock block;
       f(block);
       f(MemoryBlock());
    }
输出结果如下：

    In f(const MemoryBlock&). This version cannot modify the parameter.
    In f(MemoryBlock&&). This version can modify the parameter.
    
