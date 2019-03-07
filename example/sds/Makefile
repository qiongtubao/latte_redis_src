sds: gtest.cpp sds.c zmalloc.c
	gcc -c sds.c -o sds.o 
	gcc -c zmalloc.c -o zmalloc.o
	ar rcv sds.a sds.o zmalloc.o
	g++ -g  gtest.cpp  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include ../libs/libgtest_main.a ../libs/libgtest.a  sds.a -o sds -std=c++11
clean: 
	-rm -f *.o
	-rm -f *.a
	-rm -f sds
	-rm -f test1
	-rm -rf sds.dSYM
	-rm -rf test1.dSYM