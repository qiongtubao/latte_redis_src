#include <stdio.h>
#include <stdlib.h>
#include "zmalloc.h"
#define atomicIncr(var,count) __atomic_add_fetch(&var,(count),__ATOMIC_RELAXED)
#define atomicDecr(var,count) __atomic_sub_fetch(&var,(count),__ATOMIC_RELAXED)
//统计使用的内存数量
static size_t used_memory = 0;
#define update_zmalloc_stat_alloc(__n) do { \
    size_t _n = (__n); \
    if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); \
    atomicIncr(used_memory,__n); \
} while(0)

#define update_zmalloc_stat_free(__n) do { \
    size_t _n = (__n); \
    if (_n&(sizeof(long)-1)) _n += sizeof(long)-(_n&(sizeof(long)-1)); \
    atomicDecr(used_memory,__n); \
} while(0)

#define PREFIX_SIZE (0)
static void zmalloc_default_oom(size_t size) {
    fprintf(stderr, "zmalloc: Out of memory trying to allocate %zu bytes\n",
        size);
    fflush(stderr);
    abort();
}
static void (*zmalloc_oom_handler)(size_t) = zmalloc_default_oom;

//申请内存
void *zmalloc(size_t size) {
    void *ptr = malloc(size+PREFIX_SIZE);
    if(!ptr) {
        zmalloc_oom_handler(size);
    }
    update_zmalloc_stat_alloc(zmalloc_size(ptr));
    printf("已使用内存: %zu", used_memory);
    return ptr;
}
//释放内存
void zfree(void *ptr) {
    if (ptr == NULL) return;
    update_zmalloc_stat_free(zmalloc_size(ptr));
    free(ptr);
}
//重新申请内存
void *zrealloc(void *ptr, size_t size){
    size_t oldsize;
    void *newptr;

    if (ptr == NULL) return zmalloc(size);
    oldsize = zmalloc_size(ptr);
    newptr = realloc(ptr,size);
    if (!newptr) zmalloc_oom_handler(size);

    update_zmalloc_stat_free(oldsize);
    update_zmalloc_stat_alloc(zmalloc_size(newptr));
    return newptr;
}