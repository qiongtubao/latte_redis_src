dict: gtest.cpp dict.c ../include/zmalloc.c ../sds/sds.c
	gcc -c ../include/zmalloc.c -o zmalloc.o
	gcc -c -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include  dict.c -o dict.o 
	gcc -c ../sds/sds.c -o sds.o
	gcc -c siphash.c -o siphash.o
	ar rcv dict.a dict.o zmalloc.o sds.o siphash.o
	g++ -g  gtest.cpp  -I/Users/zhouguodong/Documents/latte_learn/latte_redis_src/example/include ../libs/libgtest_main.a ../libs/libgtest.a  dict.a -o dict -std=c++11
clean: 
	-rm -f *.o
	-rm -f *.a
	-rm -f sds
	-rm -f test1
	-rm -rf sds.dSYM
	-rm -rf test1.dSYM
	-rm -f dict