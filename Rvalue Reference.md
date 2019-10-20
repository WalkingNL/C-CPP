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
