## 右值引用的符号：&&

#### 语法
    type-id && cast-expression

#### Remarks
右值引用能够使你区分左值和右值。左值引用与右值引用在句法及语义上是相似的，但它们各自遵循一些不同的规则。更多关于左值和右值的信息，看[这里](https://docs.microsoft.com/en-us/cpp/cpp/lvalues-and-rvalues-visual-cpp?view=vs-2019)。关于左值引用的信息，看[这里](https://docs.microsoft.com/en-us/cpp/cpp/lvalue-reference-declarator-amp?view=vs-2019)。

下面的几个部分会分别描述右值引用是如何支持*移动语义*及*完美转发*的实现的。

#### 移动语义
右值引用支持*移动语义*的实现，可帮助应用程序显著提升性能。而移动语义使你编写将资源从一个对象转移到另一个对象上。
