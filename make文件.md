## Make文件

对make的使用已久，工作中经常要去维护一些复杂make文件。但总感觉对make的挖掘还远远不够。所以计划在这篇文章中，写一系列与make相关的案例。

### 一个简单的make模板

    all: test
    
    test: test1.o test2.o ...
        gcc -g -Wall tes1.c test2.c -o test -I.
        
    test1.o: test1.c
        gcc -g -Wall -c test1.c -o test1.o
    
    test2.o: test2.c
        gcc -g -Wall -c test2.c -o test2.o
  * the target `all` is nothing but a default makefile target.

