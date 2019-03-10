dict: gtest.cpp adlist.c ../include/zmalloc.c
	gcc -c ../include/zmalloc.c -o zmalloc.o
	gcc -c  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include adlist.c -o adlist.o 
	ar rcv adlist.a zmalloc.o adlist.o 
	g++ -g  gtest.cpp  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include ../libs/libgtest_main.a ../libs/libgtest.a  adlist.a -o adlist -std=c++11
clean: 
	-rm -f *.o
	-rm -f *.a
	-rm -f sds
	-rm -f test1
	-rm -rf sds.dSYM
	-rm -rf test1.dSYM
	-rm -f adlist