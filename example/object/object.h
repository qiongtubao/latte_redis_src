

#ifndef __OBJECT_H
#define __OBJECT_H

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <redisutil.h>

#define OBJ_STRING 0    /* String object. */
#define OBJ_LIST 1      /* List object. */
#define OBJ_SET 2       /* Set object. */
#define OBJ_ZSET 3      /* Sorted set object. */
#define OBJ_HASH 4      /* Hash object. */
#define OBJ_MODULE 5    /* Module object. */
#define OBJ_STREAM 6    /* Stream object. */

#define OBJ_ENCODING_RAW 0     /* Raw representation */
#define OBJ_ENCODING_INT 1     /* Encoded as integer */
#define OBJ_ENCODING_HT 2      /* Encoded as hash table */
#define OBJ_ENCODING_ZIPMAP 3  /* Encoded as zipmap */
#define OBJ_ENCODING_LINKEDLIST 4 /* No longer used: old list encoding. */
#define OBJ_ENCODING_ZIPLIST 5 /* Encoded as ziplist */
#define OBJ_ENCODING_INTSET 6  /* Encoded as intset */
#define OBJ_ENCODING_SKIPLIST 7  /* Encoded as skiplist */
#define OBJ_ENCODING_EMBSTR 8  /* Embedded sds string encoding */
#define OBJ_ENCODING_QUICKLIST 9 /* Encoded as linked list of ziplists */
#define OBJ_ENCODING_STREAM 10 /* Encoded as a radix tree of listpacks */


#define LRU_BITS 24


//in server.h
typedef struct redisObject {  //存放的对象类型
    unsigned type:4;
    //内容编码
    unsigned encoding:4;
    //与server.lruclock的时间差值
    unsigned lru:LRU_BITS; /* LRU time (relative to global lru_clock) or
                            * LFU data (least significant 8 bits frequency
                            * and most significant 16 bits access time). */
    //引用计数算法使用的引用计数器
    int refcount;
    //数据指针
    void *ptr;
} robj;

robj *createObject(int type, void *ptr);
void incrRefCount(robj *o);
void decrRefCount(robj *o);
robj *getDecodedObject(robj *o);
void freeHashObject(robj *o);
void freeSetObject(robj *o);
void freeListObject(robj *o);
void freeZsetObject(robj *o);
void freeModuleObject(robj *o);
void freeStreamObject(robj *o);


#endif