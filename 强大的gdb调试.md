## gdb到底有多强大
先看下这个[链接](http://sourceware.org/gdb/current/onlinedocs/gdb/)吧，直观的感受一下。对比我们平时经常用到的，哈哈。

#### 远程调试
接下来，我们一一道来，先从远程调试开始。因为这一块儿我也不熟悉，所以，一起来学习一下。

首先复习一下，进程调试，主要用到的命令

**attach** *process-id* // to debug a running program in the process 

**detach**; // to stop the debug

**kill**; // Kill the child process in which your program is running under GDB

