#ifndef __ZMALLOC_H
#define __ZMALLOC_H

/* Double expansion needed for stringification of macro values. */
#define __xstr(s) __str(s)
#define __str(s) #s

#include <malloc/malloc.h>
#define HAVE_MALLOC_SIZE 1
#define zmalloc_size(p) malloc_size(p)

#define ZMALLOC_LIB "libc"

void *zmalloc(size_t size);
void zfree(void *ptr);
void *zrealloc(void *ptr, size_t size);
void *zcalloc(size_t size);
#define zmalloc_usable(p) zmalloc_size(p)
#endif /* __ZMALLOC_H */