#ifndef __REDIS_UTIL_H
#define __REDIS_UTIL_H

#include <stdint.h>
#include <stdio.h>

int string2ll(const char *s, size_t slen, long long *value);
int ll2string(char *dst, size_t dstlen, long long svalue);



#endif