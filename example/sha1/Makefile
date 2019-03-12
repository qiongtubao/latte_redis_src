sha1: gtest.cpp sha1.c ../include/zmalloc.c
	gcc -c ../include/zmalloc.c -o zmalloc.o
	gcc -c  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include sha1.c -o sha1.o 
	ar rcv sha1.a zmalloc.o sha1.o 
	g++ -g  gtest.cpp  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include ../libs/libgtest_main.a ../libs/libgtest.a  sha1.a -o sha1 -std=c++11
clean: 
	-rm -f *.o
	-rm -f *.a
	-rm -f test1
	-rm -rf sha1.dSYM
	-rm -rf test1.dSYM
	-rm -f sha1