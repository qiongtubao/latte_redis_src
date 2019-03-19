SRC := /Users/zhouguodong/Documents/latte_learn/latte_redis_src/example
INCLUDELIB := $(SRC)/include
LIBS := $(SRC)/libs
OUTDIR := out
rstring: gtest.cpp  $(INCLUDELIB)/zmalloc.c $(SRC)/dict/dict.c $(SRC)/adlist/adlist.c  ../db.c  ../object.c ../../sds/sds.c rstring.c $(SRC)/dict/siphash.c ../mdb.c $(INCLUDELIB)/redisutil.c
	gcc -c  $(INCLUDELIB)/zmalloc.c -o $(OUTDIR)/zmalloc.o
	gcc -c -I$(INCLUDELIB)  $(SRC)/dict/siphash.c -o $(OUTDIR)/siphash.o
	gcc -c -I$(INCLUDELIB) $(SRC)/dict/dict.c -o $(OUTDIR)/dict.o
	gcc -c -I$(INCLUDELIB) $(SRC)/adlist/adlist.c -o $(OUTDIR)/adlist.o
	gcc -c -I$(INCLUDELIB) ../object.c -o $(OUTDIR)/object.o
	gcc -c -I$(INCLUDELIB) ../db.c -o $(OUTDIR)/db.o
	gcc -c -I$(INCLUDELIB)  ../../sds/sds.c -o $(OUTDIR)/sds.o
	gcc -c -I$(INCLUDELIB)  rstring.c  -o $(OUTDIR)/rstring.o
	gcc -c -I$(INCLUDELIB)  ../mdb.c -o $(OUTDIR)/mdb.o
	gcc -c -I$(INCLUDELIB)  $(INCLUDELIB)/redisutil.c  -o $(OUTDIR)/redisutil.o
	ar rcv $(OUTDIR)/rstring.a  $(OUTDIR)/zmalloc.o $(OUTDIR)/adlist.o $(OUTDIR)/db.o $(OUTDIR)/object.o $(OUTDIR)/sds.o $(OUTDIR)/rstring.o $(OUTDIR)/dict.o $(OUTDIR)/siphash.o $(OUTDIR)/mdb.o $(OUTDIR)/redisutil.o
	g++ -g  gtest.cpp  -I$(INCLUDELIB) $(LIBS)/libgtest_main.a $(LIBS)/libgtest.a  $(OUTDIR)/rstring.a -o rstring -std=c++11
test: test.c $(INCLUDELIB)/zmalloc.c $(SRC)/dict/dict.c $(SRC)/adlist/adlist  ../db.c  ../object.c ../../sds/sds.c rstring.c $(SRC)/dict/siphash.c $(INCLUDELIB)/redisutil.c ../mdb.c
	gcc -g -I$(INCLUDELIB) $(INCLUDELIB)/zmalloc.c $(SRC)/dict/dict.c $(SRC)/adlist/adlist  ../db.c  ../object.c ../../sds/sds.c rstring.c $(SRC)/dict/siphash.c $(INCLUDELIB)/redisutil.c ../mdb.c test.c -o test
clean: 
	-rm -rf *.o
	-rm -rf *.a
	-rm -rf $(OUTDIR)/*.a
	-rm -f rstring
	-rm -rf *.dSYM
	-rm -rf $(OUTDIR)/*.o
