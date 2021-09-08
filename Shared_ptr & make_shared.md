### 为什么官方推荐使用make_shared来初始化shared_ptr

在[网站](https://en.cppreference.com/w/cpp/memory/shared_ptr/make_shared)上看到这么一段话

* std::shared_ptr<T>(new T(args...)) performs at least two allocations (one for the object T and one for the control block of the shared pointer), while std::make_shared<T> typically performs only one allocation (the standard recommends, but does not require this; all known implementations do this)

就是说使用make_shared初始化shared_ptr，仅需要new一次，否则的话，需要new至少两次。为什么会这样呢，主要原因在于smart pointer不仅含有原始对象的空间，还包含一个叫做控制块的区域。这个控制块的区域有以下几个部分，本质上就是对原始对象的一个控制区域。所以当使用make_shared初始化时，原始对象以及控制区域会被一次性创建好，否则的话原始对象的指针以及控制块就需要单独分配，原
  
    The control block is a dynamically-allocated object that holds:

    either a pointer to the managed object or the managed object itself;
    the deleter (type-erased);
    the allocator (type-erased);
    the number of shared_ptrs that own the managed object;
    the number of weak_ptrs that refer to the managed object.
  
  总的来说，使用make_shared的话，可以减少new的次数。
  
### 使用make_shared的缺点
注：以下文字引自[这里](https://www.jianshu.com/p/03eea8262c11)

* 构造函数是保护或私有时,无法使用 make_shared
make_shared 虽好, 但也存在一些问题, 比如, 当我想要创建的对象没有公有的构造函数时, make_shared 就无法使用了。
* 对象的内存可能无法及时回收。
make_shared 只分配一次内存, 这看起来很好. 减少了内存分配的开销. 问题来了, weak_ptr 会保持控制块(强引用, 以及弱引用的信息)的生命周期, 而因此连带着保持了对象分配的内存, 只有最后一个 weak_ptr 离开作用域时, 内存才会被释放. 原本强引用减为 0 时就可以释放的内存, 现在变为了强引用, 若引用都减为 0 时才能释放, 意外的延迟了内存释放的时间. 这对于内存要求高的场景来说, 是一个需要注意的问题.
