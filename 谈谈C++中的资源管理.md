今天在Stack Overflow上看到一个有趣的[话题](https://stackoverflow.com/questions/3669833/c11-scope-exit-guard-a-good-idea)，是关于`Scope Guard`的，我们知道`Scope Guard`在处理类似异常安全这样的问题时，非常实用且易用。

## 资源管理
    内存管理太重要了，不能交给程序员来做
    内存管理太重要了，不能交给机器来做
这似乎真的成了一个悖论，正因为重要，似乎托管给谁来做，都不保险。我们知道类似Java这种强型别的语言，它正是把内存资源的释放工作交给机器来做，但也正如那句经典的话所言："免费的往往是最贵的"。关于java的内存回收可以看[这里](https://github.com/WalkingNL/JAVA_JVM/blob/master/垃圾回收.md)。

既然交给谁做都不完全可靠，索性，C++提供了针对这个问题的一个折衷方案。这个方案的原理是这样的，**先由程序员在代码中指定如何做，而由编译器来确定什么时候做**。

在解释这个原理之前，我们先回顾一个概念[RAII](https://en.cppreference.com/w/cpp/language/raii)，它要求对象必须拥有资源，也就是说对象构造的时候，需要的资源在构造时初始化。析构的时候，释放这些资源。而如今，诸如share_ptr和unique_ptr已经作为C++11的标准出现，这也意味着我们的代码中几乎不需要再写`delete`了。

但也正如这篇[文章](http://mindhacks.cn/2012/08/27/modern-cpp-practices/)所言，RAII范式虽然好，但是缺乏易用性。为此我编写了一段代码，如下所示。可以看到为关闭一个句柄，或释放一个资源，需要编写一个类。很显然这样的做法显得笨拙，所以很多时候，我们更希望直接调用相应的函数，这么一来，又回出现新的隐患，因为后续维护代码的时候，如果中间加了个`return`，就可能造成资源泄露。

    #include <mutex>
 
    template<
        typename T,
        template<typename> typename AcquirePolicy,
        template<typename> typename ReleasePolicy
    >
    class RAII
    {
    public:
        typedef T  val_type;
        typedef T& ref_type;
    
    RAII(val_type h) : m_handle(h) { AcquirePolicy<T>::Execute(m_handle); }
    RAII(const RAII&) = delete;
    RAII(RAII&&) = delete;
    RAII& operator = (const RAII&) = delete;
    ~RAII() { ReleasePolicy<T>::Execute(m_handle); }
    
    constexpr operator ref_type () { return m_handle; }
    constexpr operator ref_type () const { return m_handle; }
    
    private:
        val_type m_handle;
    };

    template<typename T> struct LockPolicy { static void Execute(T t) { t.lock(); } };
    template<typename T> struct UnlockPolicy { static void Execute(T t) { t.unlock(); } };
    template<typename T> using scope_lock = RAII<T&, LockPolicy, UnlockPolicy>;

    template<typename T> struct NoOpPolicy { static void Execute(T) {} };

    template<typename T> struct PointerReleasePolicy { static void Execute(T ptr) { delete ptr; } };
    template<typename T> struct ArrayReleasePolicy { static void Execute(T ptr) { delete[] ptr; } };

    template<typename T> using arr_ptr_handle_t = RAII<T*, NoOpPolicy, ArrayReleasePolicy>;
    template<typename T> using ptr_handle_t = RAII<T*, NoOpPolicy, PointerReleasePolicy>;

    int main(int argc, char** argv)
    {

        std::mutex m;
        scope_lock<std::mutex> lock(m);

        ptr_handle_t<int> p1 = new int;
        arr_ptr_handle_t<int> p2 = new int [2];

        *p1 = 0xDEADBEEF;
        p2[1] = 0x8BADF00D;

        return 1;
    }
所以一个比较理想的情况是像下面的样子(这段的代码的来源是[这里](http://mindhacks.cn/2012/08/27/modern-cpp-practices/))：不管以何种方式退出`foo()`，其中的`CloseHandle`都会被执行。
    
    void foo()
    {
        HANDLE h = CreateFile(...);
        ON_SCOPE_EXIT { CloseHandle(h); }
        
        ... // use the file
    }

在C++11出现之前，保障异常退出的设施在实现上并不容易，但现在确实非常的简单，如下面的代码。在`ScopeGuard`类中，定义了一个`std::function`类型的`onExitScope_`，这个方法会在析构的时候被执行到。

    class ScopeGuard
    {
    public:
        explicit ScopeGuard(std::function<void()> onExitScope) 
            : onExitScope_(onExitScope), dismissed_(false)
        { }

        ~ScopeGuard()
        {
            if(!dismissed_) // dismissed_为true时，不需要执行onExitScope_()。
            {               // 所以，如果程序正常执行，并未出现异常时，只要设置dismissed为true即可。
                onExitScope_();
            }
        }

        void Dismiss()
        {
            dismissed_ = true;
        }

    private:
        std::function<void()> onExitScope_;
        bool dismissed_;

    private: // noncopyable
        ScopeGuard(ScopeGuard const&);
        ScopeGuard& operator=(ScopeGuard const&);
    };
对于`std::function()`而言，现在开发中很多时候，它就是`lambda`函数，例如下面的代码。当对象`onExit`结束时，`CloseHandle(h)`一定被执行。

    HANDLE h = CreateFile(...);
    ScopeGuard onExit([&] { CloseHandle(h); });

