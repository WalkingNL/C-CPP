## 程序的启动优化

### 前言
桌面程序启动速度的快慢，直接影响使用者的耐心。好在目前，使用者的机器大多数性能都还不错，所以程序在启动这一环节，总体来说并没有暴露出太大的问题。但是，依然有一些重要的点，必须格外注意。因此，基于这些点，本文谈谈程序的启动优化。

### 背景介绍
不间断的有客户反应，我们程序在启动加载的过程中，出现加载时间长，加载失败等这样的问题。一开始，我们开发人员听到这样的反馈，显得异常耐心。专心检查启动代码，询问客户详细原因，甚至派驻人员去客户现场，收集相关信息等等。我们这一番折腾之后，也确实发现了一些小问题，比如偶现加载慢，但并未见到加载失败。每次面临这种情况，一般的做法就是，先慢慢观察着，要不然，也没有明确的方向指引我们该怎么做。而且，每个人手中，一大堆任务紧逼，也确实腾不出手去调查这种来历不明的问题。就这样，慢慢地，有那么一天，启动方面的问题到了终于不好意思再拖的时候了。因为不单是客户电脑上，我们开发人员的电脑上，也会在程序启动加载的时候，出现问题。

#### 问题调研
程序启动加载的过程中，除了上面说的慢以及加载失败之外，还有其它的小问题，比如假死，就是看起来一切都加载正常，界面显示没有任何问题。但当使用鼠标操作的时候，没有任何反应。有时候，这种问题，等个一段时间之后，就真的正常了。但有时候，必须得杀死进程，重新启动。不过好在，基本上二次启动时，以上的问题，就都没有了。不过不管咋样，这个问题是不能再拖了，得考虑如何解决它。

因为这个问题有性能方面的，又有程序错误处理方面的等等。所以为了防止头痛医头，脚痛医脚的情形，我们把此问题划归到了针对程序启动的性能优化上面。这么做的原因是为了一次性解决所有遇到的已知问题，并协同处理掉那些可能还未知的问题。问题划归一旦定性，指定解决问题的流程就容易很多。针对优化问题，我们的操作步骤如下：
1. 排除电脑环境以及一些人为因素的干扰。
2. 收集启动相关的代码模块。
3. 相应动态库及它们的大小。

首先，对于第一点来说，操作上确实简单，但排查周期要慢很多。因为不管咋样，这依然是个偶现的问题。为了排除不必要的环境干扰，我们重新配置了环境，并用最新的软件版本安装之后，进行观察。因为如果是程序错误触发的，问题总会显现的。当然，为了配合调研，我们在启动代码中加了很多的日志信息，以尽可能的保证，一旦出现问题，我们能直接定位到代码行。

其次，对与启动相关的代码模块，进行了分类，并且测出每一个模块，平均每一次启动需要的时间。这一个步骤中，发现了一个很难引起注意的小点。就是每次电脑重启后，首次打开我们应用程序时，所需要的加载时间总是会多一些，虽然并不明显。

最后是动态库的大小。调研这个最简单了。我们发现，其中有一个库(DLL库)的大小，有将近2M。要比我们一年多之前的版本大了有1/3。

#### 启动优化一般方案

#### 优化结果

### 后记

编译、链接、加载

链接包含两个重要的任务：符号解析及重定位

链接 发生的时机：代码静态编译、程序被加载、应用程序执行的时候


编译器：翻译源文件，产生二进制的目标代码文件

链接器与加载器的功能在某些方面有重合，主要功能有以下：
1. 程序装载：拷贝程序到内存中，以准备运行。某些情况下，仅仅是这个拷贝的工作。但某些情况下，还包含分配存储空间、设置内存保护位、映射虚拟地址到磁盘页面。

2. 重定位：为输出的目标文件中的每一部分指定一个加载地址，并修改相应的代码和数据以反映这样的变化。

3. 符号解析：由多个子程序构建一个可执行程序，子程序之间的相互引用通过符号进行。程序也可以通过符号来引用代码库中的功能。符号解析就是将**符号引用**与**符号定义**关联起来。


(1) 定义启动性能问题，包括定义启动阶段的范围和设定启动性能的可行目标；
(2) 在定义启动性能的问题之后，测试取得具体的启动性能数据；
(3) 在(1)(2)的基础上，利用性能工具确定应用程序启动的性能瓶颈或者影响启动性能的因素；
(4) 对特定的性能瓶颈或者影响因素设计具体的优化方案，并且实施优化或者实验优化方案；
(4) 针对优化后的应用程序重新回到2来确定具体的优化后的结果，然后和预期的进行比较。

这个过程可以从西面的流程图中得到体现。



测试程序性能启动的方法：
1. 保持相同的测试环境。
2. 测试电脑的OS要尽量的保持“清洁”。不要有过多的其它程序，尤其是会定期活动或者引起网络链接的程序(如防火墙)。
3. 需要每次启动OS，并且在启动后等待一段时间。知道OS的CPU占用、网络占用和IO活动都为0，内存专用量恒定，然后开始测试执行。
4. 测试所用的不同版本的应用程序安装包应该由相同的来源。
5. 测试电脑的电源管理功能，例如由的CPU有自动调整运行频率的功能。为此需要在测试时强制设置为固定频率运行。显卡也要注意类似的问题。


###### 尽量利用自动测试
自动测试更重要的能保证测试结果的精确性及稳定性


#### 优化可行文件及库文件
##### 减少动态链接库的数量
根据实践经验，如果一个应用程序启动时需要加载的动态链接库的达到数十个，则把这些动态库的数量减少到10个以内，至少可以提高启动性能达15%。
(1) 修改代码。减少不必要的动态链接库依赖尽量减少。对针对一些虽然加载，但是代码量很少的动态链接库，把启动需要的函数代码分离出来，加入到其它动态链接库中。确定，可能修改的代码量时巨大的。
(2) 合并动态链接库。把启动时需要加载的多个较小的动态链接库合并成也给大的动态库。

(3) 减小动态链接库的尺寸。清楚冗余的代码

(4) 优化可执行文件和库文件中的代码布局。优化代码布局就是通过重新把动态链接库中的函数排列得更加紧密从而达到减少IO，提高性能的目的。windows上，优化代码布局的总固体步骤分两步：第一，获取函数调用的顺序文件(.PRF，以动态链接库为单位)；第二，把这些PRF文件传递给链接器，链接器自动按照PRF文件把函数在动态链接库中的位置重新排列


#### 优化源代码
(1) 优化启动时读取的配置文件及帮助文件
(2) 预读频繁访问的文件。第一，构造相应的数据结构，把文件读入内存并且解析后将其内容放到也给内存数据结构中，以供后续读取。第二，利用操作系统的文件缓存特性，因为当前大部分硬盘自身带有一定量大小的硬件缓存。程序启动时候，直接读取一篇待缓存的文件，但并不放到具体的数据结构中。由于OS会缓该文件到内存中，从而也可以达到同样的效果并且代码量较小。在欲读时，尽量使用内存映射函数，诸如Windows的MapViewOfFile,Linux的mmap。

(3) 预加载模块
(4) 延迟加载
(5) 多线程优化启动













