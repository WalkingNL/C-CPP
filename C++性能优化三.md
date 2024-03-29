### 内联函数
总感觉`inline`成了一个被习惯性遗忘的关键字了，在学生阶段，可能都学习过它，但真正工作之后，你真的会考虑用它吗？其实在C++语言中，内联函数的引入就是出于对性能的考虑。所以如果要编写出高性能的C++程序，在语言特性层面上，是有必要仔细分析如何才能充分发挥内联的效能。

在本节，我们将会搞清楚以下几个问题：
1. 内联函数是什么？
2. 内联与虚函数
3. 内联与宏替换
4. C语言支持内联吗

#### 内联函数是什么
被调函数的函数体代码被整个的插入到调用处，而不再通过call语句进行调用，这就是内联函数。为什么要这样做呢？因为对函数进行调用，有固有的时间损耗(后面会详细分析)，而这种损耗在某些情况下是巨大的，所以需要内联函数。

怎样告诉编译器一个类成员函数要被内联呢？方式上，有两种，这里简单看一下就行。如下面的例子(示例来自[这里](https://docs.microsoft.com/en-us/cpp/cpp/inline-functions-cpp?view=vs-2019))。(1)显式的的定义，可以看到在类体外定义内联函数时，前面有`inline`关键字。(2)隐式的定义，当在类的里面声明成员函数的同时也给出了函数体，此时`inline`关键字可以省略。像`GetBalance()`方法。**为什么呢？为什么都要在有函数体的地方，才能定义**，往后看。
    
    // Inline_Member_Functions.cpp
    class Account
    {
    public:
        // 隐式的定义
        void SetAccount(double initial_balance) { balance = initial_balance; }
        double GetBalance();
        double Deposit( double Amount );
        double Withdraw( double Amount );
    private:
        double balance;
    };
    // 显式的定义
    double GetBalance() 
    { 
        return balance;
    }    
    inline double Account::Deposit( double Amount )
    {
        return ( balance += Amount );
    }

    inline double Account::Withdraw( double Amount )
    {
        return ( balance -= Amount );
    }
    int main()
    {
    }

###### 编译单元(compilation unit)与ODR原则
C++的编译是以`编译单元`为单位进行编译的，一个编译单元的大小大致和一个`.cpp`文件相当。我们知道，C++在进行编译前，还有个预处理阶段，在这个阶段，预处理会把头文件的内容完整的拷贝到`.cpp`文件的相应位置，如果有宏，还要进行宏展开等操作。等到预处理阶段真正完毕之后，编译才开始。在编译某个编译单元时，其中若有内联函数的调用，那么内联函数体必须在编译单元内。因为如果不在其中，就无法完成对调用处代码进行替代。若存在多个编译单元用到一个内联函数，按照C++规范的要求，其中的内联函数定义必须完全一致，这就是ODR(one definition rule)原则。所以，结合代码的可维护性考虑，最好将内联函数定义在头文件中，这样一来，各个编译单元中只要包含对该内联函数的`.h`文件就OK。讨论这些，只是一种建议和提醒，如果能在编码阶段就完全考虑好，后续维护起来会容易很多。

接下来，我们分析案例。如下：

    #include "account.h"

    void func()
    {
        Account a;
        a.SetAccount(100.0);
        cout << a.GetBalance();
        ......
    }
首先，进入func()时，在其栈帧中开辟空间用以存放对象a。等到进入到函数体里面，完成对对象a的构造，注意这里会调用Account默认的构造函数。然后将100.0进行压栈(因为接下来要调用函数SetAccount())，调用SetAccount函数，开辟SetAccount()函数自己的栈帧，返回时在销毁。完了之后再调用GetBlance()函数，把该函数的返回值压栈。到最后调用`cout`的`<<`操作符输出压栈的结果。这是没有内联的情况，下面看看如果内联呢，会有什么不同？

    #include "account.h"

    void func()
    {
        Account a;
        // a.SetAccount(100.0);
        a.balance = 100.0;
        
        // cout << a.GetBalance() << endl;
        double tmp = a.balance;
        cout << tmp;
        ......
    }
以上代码就是内联之后的情形，怎么样，是不是很酷！节省了函数调用必须的参数压栈、开辟栈帧及之后的各种销毁等。而且，结合代码编译器还能继续优化。如下面的代码，因为func()的终极目的就是输出设置的值，而内联就有可能让我们达到这样的目的。

    #include "account.h"

    void func()
    {
        // Account a;
        // a.SetAccount(100.0);
        // a.balance = 100.0;
        // cout << a.GetBalance() << endl;
        // double tmp = a.balance;
        cout << 100.0;
        ......
    }
当然了，这种情况属于极致的优化效果了。能达到这样的效果，是因为编译器知道足够多的上下文信息。所以，到这里，可以总结下内联函数的优点了。
1. 减少函数调用引起的开销。
2. 因为把函数体代码替换过来之后，编译器知道的上下文信息更多，分析的会更彻底。

对于第1点，函数调用时都有哪些开销呢？前面已经提到了几个，下面我们更为全面的学习一下。看下面的代码：

    void func_main()
    {
        int i = func_ord(a, b, c); // (1)
        ......                     // (2)
    }
当调用func_ord()之前，func_main()会做哪些操作呢？
> 首先，参数压栈，上面的a、b、c。如果它们是对象，还有拷贝构造的操作；
然后是保存返回地址，当函数调用结束后，要从哪里开始执行；
此外，还需要维护好当前函数栈帧中的一些寄存器信息，如堆栈指针和栈帧指针这些的。还有其他与平台相关的寄存器信息；
最后还有一些通用寄存器内容，因为通用寄存器不会区分调用者与被调用者，这有点类似全局的变量。进入被调函数后，有可能被修改，以致调用函数中的信息被覆盖。因此还需要保存它们。

上面这些工作做完了，才会调用func_ord()函数。这里需要说明两点
> 其一，首先通过移动栈指针为func_ord()函数内部声明的局部变量分配所需要的空间；其二，执行函数体内，即func_ord()内部的代码。

等到func_ord()函数执行完毕，返回时候，做的后续处理有以下几点：
> 要恢复通用寄存器的内容；恢复调用前保存的func_main()函数栈帧中的相关寄存器的内容；移动栈指针，收回func_ord()函数的栈帧；将前面保存的返回地址出栈，赋给IP寄存器；最后是移动栈指针，回收给予func_ord()函数的参数占去的空间。值得一提的是，如果入参以值传递的方式传的并且返回的是对象时，还包含对象的构造析构。如果真的是这样，调用函数会带来更大的开销。

###### 深入分析
假设进行调用函数前的准备工作与结束后的善后工作，机器指令需要的空间记为S(x)，二执行函数体内的代码需要的时间为T(x)，现在从空间、时间两个角度上来细致看下内联效果。

首先**空间**上。如果不采用内联，被调函数只有一份，直接调用即可。但内联不同，它会被所有的调用处进行复制，很显然，单纯这么看，采用内联代码会膨胀。继续看。假设有一个函数f()，内部代码大小记为D(x)，它在整个程序中，被调用n次，如果不是内联，调用f()前的准备工作与之后的善后工作会增加代码量的开销，算下来有：n * S(x) + D(x)。若是采用了内联，代码大小为n * D(x)。

    n * S(x) + D(x); // 不采用内联代码的大小
    n * D(x); // 采用内联代码的大小
对比一下这两个计算式的大小。如果假设n特别大的话，这个时候，函数体的代码又比较小，所以单纯一个D(x)就可以省略，这么一来，只需要比较D(x)与S(x)的大小。所以基本上，可以得出这样的一个结论：
> 如果内联函数体的代码量比调用函数时做的准备与善后工作引入的代码量小，那么内联后，程序整体的代码量并没有变大，甚至变得更小。就像上面的例子中展示的内联有可能对代码优化到极致，因为内联，编译器获得了更多代码的上下文信息，可以对代码的分析更为彻底；反之，如果内联函数体的代码量大于调用函数时做的准备与善后工作引入的代码量，那么内联后，程序执行整体的代码量会增大。可以看出，内联的好坏与否无法一概而论。但一个起码的指导原则是，**如果内联函数体内的代码量特别大，存在复杂循环等时**，内联并不会带来更为积极的效果。

再是**时间**上，相比于普通调用，就是非内联函数的调用，内联函数至少有以下三个方面的优势：
1. 省去了调用函数时的准备以及之后的善后工作；
2. 内联之后，编译优化工作会做的更为彻底，至少可能性更大；
3. 内联之后，代码就连成了一片，所以全部代码都在一个页或连续相邻的页中。反观普通调用，就需要跳转到调用函数所在的页面中，很可能该页面还不在物理内存。总而言之，内联后，降低了`缺页`的几率(缺页造成的影响十分大的，所以如果能减少缺页的次数，远比减少一些代码量带来的效果更佳)。还有一种情况就是，对于普通函数，就算被调函数所在的页，在内存中，但因为空间距离上与调用函数相隔甚远，可能引起缓存丢失的情况，这依然会降低执行速度。

> 上面关于时间上的分析，按照积极的一面考虑的。但极端情况下，也不能简单的一概而论。如果内联函数体内的代码特别大，而且被调用的次数又特别多。根据前面**空间**分析时提到的，这时候，内联之后，需要存放代码的页就会非常多。那么缺页现象出现的情况也会增多。最终导致程序的执行时间也会因此增多。

###### 总结
对于内联函数，表征其优势的参数有下面两点。对于实际开发中，一个函数要不要被mark为内联，内联后最终的效果如何，都需要实际测量，然后根据测量结果再进行决定。
1. 函数体内代码量；
2. 函数被调用的次数；

##### 内联函数与递归
你会不会把下面的函数定义为内联函数，或者说就算你定义成内联函数了，编译器会认可吗？所以在这里需要说明一点的就是**一个函数最终能不能内联，不能全凭程序员的个人意志，还得通过编译器的评估。如果内联导致代码膨胀太大，编译器会拒绝内联请求**。

    int Fibonacci(int n)
    {
        if (n < 0)
            return 0;
        if (n <= 1)
            return 1;
        Fibonacci(n - 1) + Fibonacci(n - 2);
    }
上面的函数到底能不能内联成功，得取决两点：
1. 递归的次数，也就是函数被调用的次数。编译器是否知道这个具体的次数；
2. 判断次数的大小。

如果编译器知道次数，并且次数通过编译器评估后可以内联，那就最终能够内联，否则不能内联。

#### 内联与虚函数
已知内联是编译时期的行为，而虚函数是动态联编，属于执行期行为。两个在不同时期执行的操作似乎并不存在交集。但仔细考虑就会发现一些端倪。虚函数因为在编译时期根本不知道到底该调用哪个虚函数，反过来说，如果在编译时期就知道具体要调用哪个虚函数，那这个问题就不存在了。所以也就是说虚函数能不能内联得取决于编译时期是否能确定到底调用哪个虚函数。这又得根据虚函数的特征。
  * 第一，通过类的对象进行调用，而不是对象指针或是对象引用。
  * 第二，尽管是通过对象指针或是对象引用调用的虚函数，但在编译时期就已经知道了具体调用的函数是哪个。

不过一般说来，虚函数被定义为内联确实有点硬搞的意思，通常也不会那么去干。原因在于我们定义虚函数，就是为了利用虚函数的特性。否则的话，是没有必要为了让虚函数也能够内联，而去强行的附和上面的条件。当然了，也不排除所有的可能，在特定条件下，真的遇到了可以让虚函数去内联，也未尝不可呢。

#### 内联与宏
内联与宏被讨论过非常多的文章，当然大多也都是为了面试。本质上两个完全不一样的东西进行比较，没有多大的意义。很多时候，把它俩拉在一起比较高低，并非出于什么性能方面的考虑e，不过是基本的考察是不是真的分别掌握这两个概念。反而越是这样，越有点小题大作。因为与其比较他俩有什么不同，不如看看他俩哪里相同了。哦，好吧，它俩都有替换的操作。说实话，就替换本身来说，也是两种完全不同的替换。当然了，如果这样的比较能让你把两个概念理解的的够透彻，倒也罢。但问题在于，这样的比较更多时候导致那些只会背诵的人，把问题搞混了。废话了多了点儿，既然到这儿了，也就比较一下吧。
1. 宏替换处于预处理时期，内联函数在编译时期；
2. 宏只是单纯的替换，无类型检查，但内联是会进行类型检查的，因为它是在编译时期的行为；
3. 宏是一定会做替换的，但内联不一定；
4. 宏在语义上会造成歧义，尤其参数里存在自增自减时，但内联不会。
5. 两者不存在所谓相互取代这一说的。为了规避内联的一些问题，而采用宏替换不是什么明智之举。倒是有一点值得一提，宏如果用好了，会超级强大。

#### 其他
main()函数可以内联吗？按照规定是不行的。本来想着展开一下，还是算了。再提一点，对于编译器默认生成的构造函数、拷贝构造函数、析构函数以及赋值运算符这些的，一般都会被内联。

C语言支持内联函数的特性吗？答案是支持。只是在声明上有点区别，`inline`关键字只能用在C++中，不过这个关键字还有两个变体，分别是`__inline`与`__forceinline`，它俩C和C++语言都支持。至于它们的区别其实也不大，都是编译时期的替换操作，`__forceinline`允许你任性，就是违背编译器的意愿，去做我就是要内联这样的事情，不过这是会付出代价的。

### 后记
关于内联函数也说完了，还是花了比较多的时间，不过相比前面临时对象，要好很多。关于内联函数，还有一些译文，后面会陆续放进来。
