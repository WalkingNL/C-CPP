## 前言
今年一直在对我们系统前后台的代码及性能做优化，感受颇深。很多时候写代码，特别容易忽略掉那些看不见的东西。本文的目标是探讨C++中那些看不见的东西之一——**临时对象**。
## 临时对象
在解释临时对象之前，先看看临时对象对性能有怎样的影响。

### 临时对象对性能的影响
对象的创建与销毁对程序性能的影响很大，尤其体积比较大的对象。比如处在复杂继承体系末端的类，或者包含很多成员变量的类。创建它们的对象，对性能影响更为显著。实际开发中，如果对这些视而不见，常常选择性的忽略，为了完成开发任务，不顾后果的任性使用，后果就是带来一大堆可见与不可见的问题。这里的"可见"与"不可见"标的都是对象。可见的对象创建与销毁，如果影响到性能，发现起来，可能相对容易很多。但是不可见的对象，由于并不会显示的体现在源码中，由编译器在编译过程中自动创建及销毁。这种"偷偷摸摸"的做法，也会影响到性能。

虽然是编译器自己产生的临时对象，但问题本身还是出在我们自己身上。先看一段代码(代码来自网络，为了说明问题而已)。可以运行输出一下，结果可能和你想象的不太一样。需要注意的是，编译环境中都开启了编译优化。所以为了看清楚效果，编译时设置编译选项`-fno-elide-constructors`，以关闭掉编译优化。
    
    #include <iostream>
    using namespace std;

    int g_constructCount=0;
    int g_copyConstructCount=0;
    int g_destructCount=0;
    struct A
    {
        A(){
            cout<<"construct: "<<++g_constructCount<<endl;    
        }

        A(const A& a)
        {
            cout<<"copy construct: "<<++g_copyConstructCount <<endl;
        }
        ~A()
        {
            cout<<"destruct: "<<++g_destructCount<<endl;
        }
    };

    A GetA()
    {
        return A();
    }

    int main() {
        A a = GetA();
        return 0;
    }

不管感觉如何，都接着往下走。我们仔细的了解学习临时对象。

### 什么是临时对象，如何产生
其实有了上面的例子，相信哪怕真的第一次听到临时对象这么个东西的人，也不会将其与临时变量混为一谈的，对吧！不过约定俗成的叫法，可能会导致一些误解，尤其对于第一次听说临时变量这个概念的人。所以，稍作解释，看下面的代码(上面代码的GetA()函数)。习惯上把整形变量`tmp_var`，也叫做一个临时变量，对吧。但是相信没人把对象`aa`称为临时对象的。我们倒是可以把它们分别称为局部变量`tmp_var`和局部对象`aa`。所以既然这种方式定义的不是临时对象，那么到底什么是临时对象？

    A GetA()
    {
        int tmp_var;
        A aa;
        
        return A();
    }

在前面就已经提到，临时对象不存在于源码中，由编译器自动产生。那么好，我们详细分析一个事例，从而掌握它。

    #include <iostream>
    #include <cstring>
    using namespace std;
    
    class Matrix
    {
    public:
        Matrix(double d = 1.0)
        {   
            cout << "Matrix::Matrix()" << endl;
            
            for (int i = 0; i < 10; i++)
            {
                for (int j = 0; j < 10; j++)
                    m[i][j] = d;
            }
        }
        Matrix(const Matrix& mt)
        {
            cout << "Matrix::Matrix(const Matrix&)" << endl;
            memcpy(this, &mt, sizeof(Matrix));
        }
        Matrix& operator=(const Matrix& mt)
        {
            if (this == &mt)
                return *this;
            
            cout << "Matrix::operator=(const Matrix)" << endl;
            memcpy(this, &mt, sizeof(Matrix));
            
            return *this;
        }
        
        friend const Matrix operator+(const Matrix&, const Matrix&);
        
    private:
        double m[10][10];
    };
    
    const Matrix operator+(const Matrix& arg1, const Matrix& arg2)
    {
        Matrix sum; //--------------(1)--------------
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 10; j++)
                sum.m[i][j] = arg1.m[i][j] + arg2.m[i][j];
        }
        
        return sum; //--------------(2)--------------
    }
    
    int main()
    {
        Matrix a(2.0), b(3.0), c; //--------------(3)--------------
        c = a + b; //--------------(4)--------------
        
        return 0;
    }
###### 分析代码
在(3)处产生3个Matrix对象，会调用3次Matrix构造；在(4)处因为运算符`+`重载，执行到(1)，再调用一次Matrix构造。返回到主函数后，因为`c = a + b`，还会执行对`=`的重载函数。所以猜测执行结果如下：

    Matrix::Matrix()
    Matrix::Matrix()
    Matrix::Matrix()
    Matrix::Matrix()
    Matrix::operator=(const Matrix)
包含四次构造，一次对运算符`=`的重载。编译运行后，实际的运行结果呢？如下所示。多了一次拷贝构造的执行。一般而言，拷贝构造函数的调用不应该是，如`Matrix m = a`，类似这样的形式吗？但是代码中，除了`c = a + b`，因为运算符`=`的存在，貌似可能执行拷贝构造，就再没有任何代码了。不过，上文已经明确，这里调用的是对运算符`=`的重载函数。如果细心推测，就不难发现拷贝构造的执行一定在代码(1)之后的某条语句。那么就只有(2)了。当然，一定不是(1)(2)之间的循环。

    Matrix::Matrix()
    Matrix::Matrix()
    Matrix::Matrix()
    Matrix::Matrix()
    Matrix::Matrix(const Matrix&)
    Matrix::operator=(const Matrix)
代码(2)处表示将对象sum返回。对，就是在返回这个对象时，编译器生成了一个"临时对象"。why？这个问题就在于sum是一个临时变量，生命周期要结束了，意味着sum要被销毁了。所以返回的对象就得在当前函数的栈中开辟空间用来存放返回值。这个临时的Matrix对象就是`a + b`返回时通过拷贝构造函数构造。

###### 临时对象的产生
上面已经提及了，返回值为一个对象时，会产生临时对象，这只是一种情况；还有另一种情况就是函数调用时，实参类型与函数定义中的参数类型不一致。

### 如何避免

### 后记






