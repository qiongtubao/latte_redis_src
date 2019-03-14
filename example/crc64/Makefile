dict: gtest.cpp crc64.c ../include/zmalloc.c
	gcc -c ../include/zmalloc.c -o zmalloc.o
	gcc -c  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include crc64.c -o crc64.o 
	ar rcv crc64.a zmalloc.o crc64.o 
	g++ -g  gtest.cpp  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include ../libs/libgtest_main.a ../libs/libgtest.a  crc64.a -o crc64 -std=c++11
clean: 
	-rm -f *.o
	-rm -f *.a
	-rm -f sds
	-rm -f test1
	-rm -rf sds.dSYM
	-rm -rf test1.dSYM
	-rm -f crc64