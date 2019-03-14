#include <stdint.h>
#include <unistd.h>
#include "zmalloc.h"
#ifndef __DICT_H
#define __DICT_H

#define DICT_OK 0
#define DICT_ERR 1
void tserverAssert(const char *estr, const char *file, int line); 
#define assert(_e) ((_e)?(void)0 : (tserverAssert(#_e,__FILE__,__LINE__),_exit(1)))

/**
 * hash节点
 */
typedef struct dictEntry {
    void *key; //键
    union { 
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    } v; //值
    struct dictEntry *next; //指向下一个节点
} dictEntry;

typedef struct dictType {
    uint64_t (*hashFunction)(const void *key);
    void *(*keyDup)(void *privdata, const void *key);
    void *(*valDup)(void *privdata, const void *obj);
    int (*keyCompare)(void *privdata, const void *key1, const void *key2);
    void (*keyDestructor)(void *privdata, void *key);
    void (*valDestructor)(void *privdata, void *obj);
} dictType;

//hash表
typedef struct dictht {
    dictEntry **table;
    unsigned long size;
    unsigned long sizemask;
    unsigned long used;
} dictht;
//字典
typedef struct dict {
    dictType *type; //类型处理函数
    void *privdata; //类型处理函数私有值
    dictht ht[2]; //两个hash表
    long rehashidx; //rehash标示，为－1表示不在rehash，不为0表示正在rehash的桶
    unsigned long iterators; //当前正在运行的安全迭代器数量
} dict;


//创建字典
dict *dictCreate(dictType *type,
        void *privDataPtr);
#define DICT_HT_INITIAL_SIZE     4

//字典添加key value
int dictAdd(dict *d, void *key, void *val);
dictEntry *dictFind(dict *d, const void *key);
//删除字典里key
int dictDelete(dict *ht, const void *key);
void dictRelease(dict *d);
uint64_t siphash(const uint8_t *in, const size_t inlen, const uint8_t *k);
uint64_t siphash_nocase(const uint8_t *in, const size_t inlen, const uint8_t *k);
uint64_t dictGenHashFunction(const void *key, int len);
//创建hashkey
#define dictHashKey(d, key) (d)->type->hashFunction(key)
//检测字典是否有扩充
#define dictIsRehashing(d) ((d)->rehashidx != -1)
//获得字典长度
#define dictSize(d) ((d)->ht[0].used+(d)->ht[1].used)
//修改或添加字典里的值
#define dictSetVal(d, entry, _val_) do { \
    if ((d)->type->valDup) \
        (entry)->v.val = (d)->type->valDup((d)->privdata, _val_); \
    else \
        (entry)->v.val = (_val_); \
} while(0)
//比较2个key值
#define dictCompareKeys(d, key1, key2) \
    (((d)->type->keyCompare) ? \
        (d)->type->keyCompare((d)->privdata, key1, key2) : \
        (key1) == (key2))

#define dictSetKey(d, entry, _key_) do { \
    if ((d)->type->keyDup) \
        (entry)->key = (d)->type->keyDup((d)->privdata, _key_); \
    else \
        (entry)->key = (_key_); \
} while(0)

#define dictFreeKey(d, entry) \
    if ((d)->type->keyDestructor) \
        (d)->type->keyDestructor((d)->privdata, (entry)->key)
        
#define dictFreeVal(d, entry) \
    if ((d)->type->valDestructor) \
        (d)->type->valDestructor((d)->privdata, (entry)->v.val)


void _serverAssert(const char *estr, const char *file, int line);

#define DICT_NOTUSED(V) ((void) V)

#endif /* __DICT_H */