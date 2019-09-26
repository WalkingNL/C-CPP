## Make文件

    all: test
    
    test: test1.o test2.o ...
        gcc -g -Wall test1.c test2.c -o test -I.
        
    test1.o: test1.c
        gcc -g -Wall -c test1.c -o test1.o
    
    test2.o: test2.c
        gcc -g -Wall -c test2.c -o test2.o
  * the target `all` is nothing but a default makefile target.

