dict: gtest.cpp intset.c ../include/zmalloc.c
	gcc -c ../include/zmalloc.c -o zmalloc.o
	gcc -c  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include intset.c -o intset.o 
	ar rcv intset.a zmalloc.o intset.o 
	g++ -g  gtest.cpp  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include ../libs/libgtest_main.a ../libs/libgtest.a  intset.a -o intset -std=c++11
clean: 
	-rm -f *.o
	-rm -f *.a
	-rm -f test1
	-rm -rf intset.dSYM
	-rm -rf test1.dSYM
	-rm -f intset