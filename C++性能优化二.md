### 临时对象

##### 对象作为函数的返回值

    #include <iostream>
    
    class A
    {
        friend const A operator+(const A&, const A&);
      public:
        A (int a = 0, int b = 1) : m(a), b(b)
        {
            cout << "A::A(int, int)" << endl;
        }
        A (const A& r) : m(r.m), n(r.n)
        {
            cout << "A::A(const A&)" << endl;
        }
        A& operator=(const A& r)
        {
            if (this == &r)
                return (*this);
            m = r.m;
            n = r.n;
            cout << "A::operator=(const A&)" << endl;
            return (*this);
        }
    
    private:
        int m;
        int n;
    };
    
    const A operator+(const A& a, const A& b)
    {
        cout << "operator+() start" << endl;
        A tmp; //-----------(3)------------
        tmp.m = a.m + b.n;
        tmp.n = a.n + b.;
        cout << "operator+() end" << endl;
        return tmp; //-----------(4)------------
    }
    
    int main(int argc, char* argv[])
    {
        A r, a(10, 10), b(5, 0) //-----------(1)------------
        r = a + b; //-----------(2)------------
        
        return 0;
    }
执行到(2)时，调用`+`重载函数。注意这个时候会在main()函数的栈帧中开辟一块空间，大小就是一个类A对象占用的空间。开辟这个空间的作用是存放(4)处的返回值的。上一篇有说过，因为tmp是临时变量，`+`重载函数被执行完毕之后，要被销毁的。所以在销毁前，首先会将tmp的实例空间拷贝构造一份，生成类A的一个临时对象放在main()开辟的栈帧空间中。然后在执行(2)处代码的`=`部分。这段话里有几个点，需要特别小心，我再重述以下，见下。

    语句：r = a + b;
    (1) 首先执行+运算符重载函数
    (2) 在main()中开辟空间，为了存放返回的临时对象。注意是在调用者中开辟，即main()中开辟，非被调用者。
    (3) 调用拷贝构造，生成临时对象
    (4) 执行赋值运算符
这个过程，输出如下：

    A::A(int, int) // (1)处代码的输出
    A::A(int, int) // (1)处代码的输出
    A::A(int, int) // (1)处代码的输出
    operator+() start // (2)处代码的输出，执行运算符+的重载函数
    A::A(int, int) // (3)处代码的输出，因为要构造临时变量 tmp
    operator+() end // +的重载函数
    A::A(const A&) // (4)处代码的输出
    A::operator=(const A&) // (2)处代码的输出，因为需要执行=运算符的重载函数
有个问题，我相信每一个人都遇到过。就是声明的类A的`r`对象。是否有想过对于非built-in类型的变量来说，还是先看下面的代码吧。这两种情况有什么不同呢？除了代码的表现形式之外，实际执行起来会有差别吗？

    // 第一种情况
    A r, a(10, 10), b(5, 0); 
    r = a + b;
    
    // 第二种情况
    A a(10, 10), b(5, 0);
    A r = a + b;
第一种情况，上面的结果已经有了，那么第二种情况呢，看我下面的执行结果

  ~~A::A(int, int)~~
  
  A::A(int, int)
  
  A::A(int, int)
  
  operator+() start
  
  A::A(int, int)
  
  operator+() end
  
  A::A(const A&)
  
  ~~A::operator=(const A&)~~
