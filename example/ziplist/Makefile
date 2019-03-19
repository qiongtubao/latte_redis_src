ziplist: gtest.cpp ziplist.c ../include/zmalloc.c ../include/redisassert.c ../include/redisutil.c
	gcc -c ../include/zmalloc.c  -o zmalloc.o
	gcc -c ../include/redisassert.c -o redisassert.o
	gcc -c ../include/redisutil.c -o redisutil.o
	gcc -c  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include ziplist.c -o ziplist.o 
	ar rcv ziplist.a zmalloc.o ziplist.o  redisassert.o redisutil.o
	g++ -g  gtest.cpp  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include ../libs/libgtest_main.a ../libs/libgtest.a  ziplist.a -o ziplist -std=c++11
clean: 
	-rm -f *.o
	-rm -f *.a
	-rm -f test1
	-rm -rf ziplist.dSYM
	-rm -rf test1.dSYM
	-rm -f ziplist