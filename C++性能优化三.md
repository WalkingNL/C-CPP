### 内联函数
总感觉`inline`成了一个被习惯性遗忘的关键字了，在学生阶段，可能都学习过它，但真正工作之后，你真的会考虑用它吗？其实在C++语言中，内联函数的引入就是出于对性能的考虑。所以如果要编写出高性能的C++程序，在语言特性层面上，是有必要仔细分析如何才能充分发挥内联的效能。

在本节，我们将会搞清楚以下几个问题：
1. 内联函数是什么？
2. 内联VS宏替换，谁是宠儿？
3. 内联函数，什么时候需要？
4. main()可以内联吗？


#### 内联函数是什么
被调函数的函数体代码被整个的插入到调用处，而不再通过call语句进行调用，这就是内联函数。为什么要这样做呢？因为对函数进行调用，有固有的时间损耗(后面会详细分析)，而这种损耗在某些情况下是巨大的，所以需要内联函数。

    // Inline_Member_Functions.cpp
    class Account
    {
    public:
        Account(double initial_balance) { balance = initial_balance; }
        double GetBalance();
        double Deposit( double Amount );
        double Withdraw( double Amount );
    private:
        double balance;
    };

    inline double Account::GetBalance()
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

C语言支持内联函数的特性吗？答案是支持。只是在声明上有点区别，`inline`关键字只能用在C++中，不过这个关键字还有两个变体，分别是`__inline`与`__forceinline`，它俩C和C++语言都支持。至于它们的区别其实也不大，我们放在后面说吧。

将被调用内联函数的函数题代码直接替代对该内联函数的调用
