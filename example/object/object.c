#include "object.h"
#define MAXMEMORY_FLAG_LRU (1<<0)
#define MAXMEMORY_FLAG_LFU (1<<1)
#define LFU_INIT_VAL 5
#define OBJ_SHARED_REFCOUNT INT_MAX
#include <zmalloc.h>

robj *createObject(int type, void *ptr) {
    robj *o = zmalloc(sizeof(*o));
    o->type = type;
    o->encoding = OBJ_ENCODING_RAW;
    o->ptr = ptr;
    o->refcount = 1;
    //LRU 机制
    // if(server.maxmemory_policy & MAXMEMORY_FLAG_LFU) {
    //     o->lru = (LFUGetTimeInMinutes() << 8) | LFU_INIT_VAL;
    // } else {
    //     o->lru = LRU_CLOCK();
    // }
    return o;
}

//添加计数器
void incrRefCount(robj *o) {
    if (o->refcount != OBJ_SHARED_REFCOUNT) o->refcount++;
}

//减少计数器
void decrRefCount(robj *o) {
    if (o->refcount == 1) {
        switch(o->type) {
        case OBJ_STRING: freeStringObject(o); break;
        case OBJ_LIST: freeListObject(o); break;
        case OBJ_SET: freeSetObject(o); break;
        case OBJ_ZSET: freeZsetObject(o); break;
        case OBJ_HASH: freeHashObject(o); break;
        case OBJ_MODULE: freeModuleObject(o); break;
        case OBJ_STREAM: freeStreamObject(o); break;
        default:  printf("Unknown object type"); break;
        }
        zfree(o);
    } else {
        if (o->refcount <= 0)  printf("decrRefCount against refcount <= 0");
        if (o->refcount != OBJ_SHARED_REFCOUNT) o->refcount--;
    }
}
#define sdsEncodedObject(objptr) (objptr->encoding == OBJ_ENCODING_RAW || objptr->encoding == OBJ_ENCODING_EMBSTR)
robj *getDecodedObject(robj *o) {
    robj *dec;

    if (sdsEncodedObject(o)) {
        incrRefCount(o);
        return o;
    }
    if (o->type == OBJ_STRING && o->encoding == OBJ_ENCODING_INT) {
        char buf[32];

        ll2string(buf,32,(long)o->ptr);
        dec = createStringObject(buf,strlen(buf));
        return dec;
    } else {
        //  printf("Unknown encoding type");
    }
}

void freeHashObject(robj *o) {
    // switch (o->encoding) {
    // case OBJ_ENCODING_HT:
    //     dictRelease((dict*) o->ptr);
    //     break;
    // case OBJ_ENCODING_ZIPLIST:
    //     zfree(o->ptr);
    //     break;
    // default:
    //     printf("Unknown hash encoding type");
    //     break;
    // }
}

void freeListObject(robj *o) {
    // if (o->encoding == OBJ_ENCODING_QUICKLIST) {
    //     quicklistRelease(o->ptr);
    // } else {
    //     printf("Unknown list encoding type");
    // }
}

void freeSetObject(robj *o) {
    // switch (o->encoding) {
    // case OBJ_ENCODING_HT:
    //     dictRelease((dict*) o->ptr);
    //     break;
    // case OBJ_ENCODING_INTSET:
    //     zfree(o->ptr);
    //     break;
    // default:
    //      printf("Unknown set encoding type");
    // }
}

void freeZsetObject(robj *o) {
    // zset *zs;
    // switch (o->encoding) {
    // case OBJ_ENCODING_SKIPLIST:
    //     zs = o->ptr;
    //     dictRelease(zs->dict);
    //     zslFree(zs->zsl);
    //     zfree(zs);
    //     break;
    // case OBJ_ENCODING_ZIPLIST:
    //     zfree(o->ptr);
    //     break;
    // default:
    //      printf("Unknown sorted set encoding");
    // }
}

void freeModuleObject(robj *o) {
    // moduleValue *mv = o->ptr;
    // mv->type->free(mv->value);
    // zfree(mv);
}

void freeStreamObject(robj *o) {
    // freeStream(o->ptr);
}
