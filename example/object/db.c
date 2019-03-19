#include "db.h"
#include <stdio.h>

void signalKeyAsReady(redisDb *db, robj *key) {
    readyList *rl;
    //阻塞keys是否有key
    if (dictFind(db->blocking_keys,key) == NULL) return;
    //存储key值 
    if (dictFind(db->ready_keys,key) != NULL) return;
    rl = zmalloc(sizeof(*rl));
    rl->key = key;
    rl->db = db;
    // //引用计数 +1
    // incrRefCount(key);
    // listAddNodeTail(server.ready_keys,rl);
    //引用计数 +1
    incrRefCount(key);
    // serverAssert(dictAdd(db->ready_keys,key,NULL) == DICT_OK);

}

void dbAdd(redisDb *db, robj *key, robj *val) {
    //复制字符串
    sds copy = sdsdup(key->ptr);
    int retval = dictAdd(db->keyspace, copy, val);
    // serverAssertWidthInfo(NULL, key, retval == DICT_OK);
    
    if(val -> type == OBJ_LIST || 
        val -> type == OBJ_ZSET) {
            // 如果是list 或者zset 对象 判断是否有阻塞命令在监听  阻塞BLPOP有没有
            signalKeyAsReady(db, key);
        }
    // 集群相关操作
    // if(server.cluster_enabled) {
    //     
    //     slotToKeyAdd(key);
    // }
}