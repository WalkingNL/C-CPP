
## 线程池
目前，网络上，关于线程池的文章太多了，不过这些文章无一例外讨的都是一些通用的线程池。而往往通用的东西，因为需要考虑各种问题，所以在性能方面，做了一些不得已的取舍。但在项目开发中，当一些问题确定不会出现的时候，强行的套用这些通用的设计方案，并不总是都是好的。所以，在本篇文章中，将讨论如何根据业务的具体需要，设计一个契合自己项目要求的线程池。因为只有这样的讨论，才能避免许多偷懒的做法，有助与彻底理解一个线程池。

当然啦，从个人的经历来说，从头设计并实现一个线程池虽非难事儿，但也绝非易事儿，即使是那种最简单的只需要固定线程数的线程池。更不用你说，在性能、适用场景等方面尽善尽美的那种，如需要根据程序运行情况，动态调整线程数的、支持线程存活时间的、任务队列有长度限制的、甚至还有那种为了复用资源，可以对线程池进行合并(拆分)的、支持定时任务的、批量提交的等等。

这篇文章计划将从以下两个大的主题入手：
1. 线程数目固定的线程池
2. 线程数目可变的线程池

本篇文章使用的开发语言主要是c++，不过在第二部分会涉及到Java，主要是想借助Java语言里面的线程池，讨论设计一个数量不固定的线程池需要考虑的一些必要因素。

#### 预备知识点
虽然我们项目中，至今并没有用到C++11及更高版本的内容，但本文不打算这么做。所以，会尽可能的应用C++中，一些新的、比较高级的机制。
  * [条件变量](https://en.cppreference.com/w/cpp/thread/condition_variable)
  * [互斥锁]()

对于条件变量(condition variables)，它是同步原语(synchronization primitive)，使线程处于等待的状态，直到一个特定的条件的发生

##### 条件变量

### 线程数目固定的线程池

下面的代码，来自知乎上一篇介绍线程池的[文章](https://www.zhihu.com/question/27908489/answer/355105668)。其中，给出了如何实现一个固定数量的线程池。

    #include <mutex>
    #include <condition_variable>
    #include <functional>
    #include <queue>
    #include <thread>

    class fixed_thread_pool {
     public:
      explicit fixed_thread_pool(size_t thread_count)
          : data_(std::make_shared<data>()) {
        for (size_t i = 0; i < thread_count; ++i) {
          std::thread([data = data_] {
            std::unique_lock<std::mutex> lk(data->mtx_);
            for (;;) {
              if (!data->tasks_.empty()) {
                auto current = std::move(data->tasks_.front());
                data->tasks_.pop();
                lk.unlock();
                current();
                lk.lock();
              } else if (data->is_shutdown_) {
                break;
              } else {
                data->cond_.wait(lk);
              }
            }
          }).detach();
        }
      }

      fixed_thread_pool() = default;
      fixed_thread_pool(fixed_thread_pool&&) = default;

      ~fixed_thread_pool() {
        if ((bool) data_) {
          {
            std::lock_guard<std::mutex> lk(data_->mtx_);
            data_->is_shutdown_ = true;
          }
          data_->cond_.notify_all();
        }
      }

      template <class F>
      void execute(F&& task) {
        {
          std::lock_guard<std::mutex> lk(data_->mtx_);
          data_->tasks_.emplace(std::forward<F>(task));
        }
        data_->cond_.notify_one();
      }

     private:
      struct data {
        std::mutex mtx_;
        std::condition_variable cond_;
        bool is_shutdown_ = false;
        std::queue<std::function<void()>> tasks_;
      };
      std::shared_ptr<data> data_;
    };
    
    
    ### 线程数目可变的的线程池

