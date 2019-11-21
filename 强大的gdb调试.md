## gdb到底有多强大
先看下这个[链接](http://sourceware.org/gdb/current/onlinedocs/gdb/)吧，直观的感受一下。对比我们平时经常用到的，哈哈。

#### 远程调试
接下来，我们一一道来，先从远程调试开始。因为这一块儿我也不熟悉，所以，一起来学习一下。

首先复习一下，进程调试，主要用到的命令：
###### 多进程
**attach** *process-id* // to debug a running program in the process 

**detach**; // to stop the debug

**kill**; // Kill the child process in which your program is running under GDB

**info inferiors**

**inferior** *inferiors*

**add-inferior \[ -copies n ] [ -exec executable ]**及*****clone-inferior [ -copies n ] \[ infno ]***，这两个命令可以用来在一个调试session中，添加(或克隆)多个进程在其中。。

对比添加进程的命令，就有删除的命令，**remove-inferiors***infno*。需要注意的是对于处于运行当中的进程，无法起效。得尝试用*kill*或*detach*命令。

此外，还要知道调试器(gdb)提供的[convenience variable](https://sourceware.org/gdb/current/onlinedocs/gdb/Convenience-Vars.html#Convenience-Vars)机制，如`$_inferior`。

###### 多线程
**thread*thread_id***, // 多个线程之间的切换

**info threads**, // 查询现有线程


