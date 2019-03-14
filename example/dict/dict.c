

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <sys/time.h>
#include "dict.h"
#include "zmalloc.h"
static int _dictInit(dict *ht, dictType *type, void *privDataPtr);
static void _dictReset(dictht *ht);
static uint8_t dict_hash_function_seed[16];
static void _dictRehashStep(dict *d);
static unsigned long _dictNextPower(unsigned long size);
static dictEntry *dictGenericDelete(dict *d, const void *key, int nofree);

void tserverAssert(const char *estr, const char *file, int line){
    fprintf(stderr, "=== ASSERTION FAILED ===");
    fprintf(stderr, "==> %s:%d '%s' is not true",file,line,estr);
    *((char*)-1) = 'x';
}
/* 
  * 返回可以填充的空闲槽的索引
  * 给定'key'的哈希条目。
  * 如果密钥已存在，则返回-1
  * 和可选的输出参数可以填写。
 *
  * 请注意，如果我们正在重新散列哈希表，那么
  * index始终在第二个（新）哈希表的上下文中返回。
* */
static long _dictKeyIndex(dict *d, const void *key, uint64_t hash, dictEntry **existing);
/*
执行N步增量重复。 如果还有，则返回1
  *键从旧哈希表移动到新哈希表，否则返回0。
 *
  *请注意，重新移动步骤包括移动一个桶（可能有更多
  但是，从旧的哈希表到新的哈希表，比我们使用链接的一个键
  *由于哈希表的一部分可能由空格组成，因此不是
  *保证这个功能甚至可以重新连接一个桶，因为它
  *总共将访问最多N * 10个空桶，否则金额为
  *它的工作将是未绑定的，并且该功能可能会阻塞很长时间。
*/
int dictRehash(dict *d, int n) {
    int empty_visits = n*10; /* Max number of empty buckets to visit. */
    if (!dictIsRehashing(d)) return 0;

    while(n-- && d->ht[0].used != 0) {
        dictEntry *de, *nextde;

        /* Note that rehashidx can't overflow as we are sure there are more
         * elements because ht[0].used != 0 */
        assert(d->ht[0].size > (unsigned long)d->rehashidx);
        while(d->ht[0].table[d->rehashidx] == NULL) {
            d->rehashidx++;
            if (--empty_visits == 0) return 1;
        }
        de = d->ht[0].table[d->rehashidx];
        /* Move all the keys in this bucket from the old to the new hash HT */
        while(de) {
            uint64_t h;

            nextde = de->next;
            /* Get the index in the new hash table */
            h = dictHashKey(d, de->key) & d->ht[1].sizemask;
            de->next = d->ht[1].table[h];
            d->ht[1].table[h] = de;
            d->ht[0].used--;
            d->ht[1].used++;
            de = nextde;
        }
        d->ht[0].table[d->rehashidx] = NULL;
        d->rehashidx++;
    }

    /* Check if we already rehashed the whole table... */
    if (d->ht[0].used == 0) {
        zfree(d->ht[0].table);
        d->ht[0] = d->ht[1];
        _dictReset(&d->ht[1]);
        d->rehashidx = -1;
        return 0;
    }

    /* More to rehash... */
    return 1;
}
/* 返回大于size的二次幂值 */
static unsigned long _dictNextPower(unsigned long size)
{
    unsigned long i = DICT_HT_INITIAL_SIZE;

    if (size >= LONG_MAX) return LONG_MAX + 1LU;
    while(1) {
        if (i >= size)
            return i;
        i *= 2;
    }
}
/*
    此函数仅执行重新散列的步骤，并且仅在存在时执行
  *没有安全的迭代器绑定到我们的哈希表。 当我们有迭代器的时候
  *中间的一个rehashing我们不能乱用两个哈希表否则
  *某些元素可能会被遗漏或重复。
 *
  *此函数通过常见的查找或更新操作调用
  *字典，以便哈希表自动从H1迁移到H2
  *虽然它被积极使用。
*/
static void _dictRehashStep(dict *d) {
    if (d->iterators == 0) dictRehash(d,1);
}
//创建字典
dict *dictCreate(dictType *type,
        void *privDataPtr)
{
    dict *d = zmalloc(sizeof(*d));

    _dictInit(d,type,privDataPtr);
    return d;
}

uint64_t dictGenHashFunction(const void *key, int len) {
    return siphash(key,len,dict_hash_function_seed);
}

static void _dictReset(dictht *ht)
{
    ht->table = NULL;
    ht->size = 0;
    ht->sizemask = 0;
    ht->used = 0;
}
//字典对象初始化
int _dictInit(dict *d, dictType *type,
        void *privDataPtr)
{
    _dictReset(&d->ht[0]);
    _dictReset(&d->ht[1]);
    d->type = type;
    d->privdata = privDataPtr;
    d->rehashidx = -1;
    d->iterators = 0;
    return DICT_OK;
}
//扩充桶容量
int dictExpand(dict *d, unsigned long size)
{
    /* 在扩充或者已经比将要扩充的大小要大了的话 返回失败*/
    if (dictIsRehashing(d) || d->ht[0].used > size)
        return DICT_ERR;

    dictht n; /* 新的hash表 */
    unsigned long realsize = _dictNextPower(size);

    /* 重新使用相同的表大小是没有用的。 */
    if (realsize == d->ht[0].size) return DICT_ERR;

    /* 分配新的哈希表并将所有指针初始化为NULL*/
    n.size = realsize;
    n.sizemask = realsize-1;
    n.table = (struct dictEntry**)zcalloc(realsize*sizeof(dictEntry*));
    n.used = 0;

    /* 这是第一次初始化吗？ 如果是这样的话，那真的不是重复
      *我们只是设置第一个哈希表，以便它可以接受密钥。 */
    if (d->ht[0].table == NULL) {
        d->ht[0] = n;
        return DICT_OK;
    }

    /* 准备第二个哈希表以进行增量重组*/
    d->ht[1] = n;
    d->rehashidx = 0;
    return DICT_OK;
}
static int dict_can_resize = 1;
static unsigned int dict_force_resize_ratio = 5;
//是否需要扩充
static int _dictExpandIfNeeded(dict *d) {
    /* 增量重组已在进行中。 返回。*/
    if (dictIsRehashing(d)) return DICT_OK;

    /* hash为空  将其扩展为初始化大小 */
    if (d->ht[0].size == 0) return dictExpand(d, DICT_HT_INITIAL_SIZE);

    /* 如果我们达到1：1的比例，我们可以调整哈希值
      * 表（全局设置）或者我们应该避免它但是它们之间的比例
      * 元素/桶超过“安全”门槛，我们调整倍增
      * 水桶的数量。 */
    if (d->ht[0].used >= d->ht[0].size &&
        (dict_can_resize ||
         d->ht[0].used/d->ht[0].size > dict_force_resize_ratio))
    {
        return dictExpand(d, d->ht[0].used*2);
    }
    return DICT_OK;
}
//查找可用空闲槽索引 
static long _dictKeyIndex(dict *d, const void *key, uint64_t hash, dictEntry **existing)
{
    unsigned long idx, table;
    dictEntry *he;
    if (existing) *existing = NULL;

    /* Expand the hash table if needed */
    //是否扩展哈希表
    if (_dictExpandIfNeeded(d) == DICT_ERR)
        return -1;
    for (table = 0; table <= 1; table++) {
        idx = hash & d->ht[table].sizemask;
        /* Search if this slot does not already contain the given key */
        he = d->ht[table].table[idx];
        while(he) {
            //比较是否相同  如果已经存在的话返回-1
            if (key==he->key || dictCompareKeys(d, key, he->key)) {
                if (existing) *existing = he;
                return -1;
            }
            he = he->next;
        }
        if (!dictIsRehashing(d)) break;
    }
    return idx;
}
//创建字典数据节点对象
dictEntry *dictAddRaw(dict *d, void *key, dictEntry **existing) {
    long index;
    dictEntry *entry;
    dictht *ht;
    if(dictIsRehashing(d)) {
        _dictRehashStep(d);
    }
    //判断是否有可用空闲槽索引 
    if((index = _dictKeyIndex(d, key, dictHashKey(d, key), existing)) == -1) {
        //插入数据失败
        return NULL;
    }
    //还在rehash？  如果是是的话选用ht[1]  
    ht = dictIsRehashing(d) ? &d->ht[1]: &d ->ht[0];
    //创建一个空的entry
    entry = zmalloc(sizeof(*entry));
    //next设置为原来位置上的对象
    entry->next = ht->table[index];
    ht->table[index] = entry;
    //增加使用个数
    ht->used++;
    dictSetKey(d, entry, key);
    return entry;
}
//字典添加新值
int dictAdd(dict *d, void *key, void *val) {
   dictEntry *entry = dictAddRaw(d, key, NULL);
   if(!entry) {
       return DICT_ERR;
   }
   dictSetVal(d, entry, val);
   return DICT_OK;
}

/**
 *  查找对象
 */ 
dictEntry *dictFind(dict *d, const void *key) {
    dictEntry *he;
    uint64_t h, idx, table;
    if (d->ht[0].used + d->ht[1].used == 0) return NULL; /* dict is empty */
    if (dictIsRehashing(d)) _dictRehashStep(d);
    h = dictHashKey(d, key);
    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];
        while(he) {
            if (key==he->key || dictCompareKeys(d, key, he->key))
                return he;
            he = he->next;
        }
        if (!dictIsRehashing(d)) return NULL;
    }
    return NULL;
}
static dictEntry *dictGenericDelete(dict *d, const void *key, int nofree) {
    uint64_t h, idx;
    dictEntry *he, *prevHe;
    int table;

    if (d->ht[0].used == 0 && d->ht[1].used == 0) return NULL;

    if (dictIsRehashing(d)) _dictRehashStep(d);
    h = dictHashKey(d, key);

    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];
        prevHe = NULL;
        while(he) {
            if (key==he->key || dictCompareKeys(d, key, he->key)) {
                /* Unlink the element from the list */
                if (prevHe)
                    prevHe->next = he->next;
                else
                    d->ht[table].table[idx] = he->next;
                if (!nofree) {
                    dictFreeKey(d, he);
                    dictFreeVal(d, he);
                    zfree(he);
                }
                d->ht[table].used--;
                return he;
            }
            prevHe = he;
            he = he->next;
        }
        if (!dictIsRehashing(d)) break;
    }
    return NULL; /* not found */
}
int dictDelete(dict *ht, const void *key) {
    return dictGenericDelete(ht,key,0) ? DICT_OK : DICT_ERR;
}
int _dictClear(dict *d, dictht *ht, void(callback)(void *)) {
    unsigned long i;

    /* Free all the elements */
    for (i = 0; i < ht->size && ht->used > 0; i++) {
        dictEntry *he, *nextHe;

        if (callback && (i & 65535) == 0) callback(d->privdata);

        if ((he = ht->table[i]) == NULL) continue;
        while(he) {
            nextHe = he->next;
            dictFreeKey(d, he);
            dictFreeVal(d, he);
            zfree(he);
            ht->used--;
            he = nextHe;
        }
    }
    /* Free the table and the allocated cache structure */
    zfree(ht->table);
    /* Re-initialize the table */
    _dictReset(ht);
    return DICT_OK; /* never fails */
}

void dictRelease(dict *d) {
    _dictClear(d,&d->ht[0],NULL);
    _dictClear(d,&d->ht[1],NULL);
    zfree(d);
}