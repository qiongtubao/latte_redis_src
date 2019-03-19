
#ifndef __RSTRING_H
#define __RSTRING_H

#include "../object.h"
#include "../db.h"
#include <stdio.h>
#include <redisutil.h>


robj *createStringObject(const char *ptr, size_t len);
robj *createEmbeddedStringObject(const char *ptr, size_t len);
robj* createRawStringObject(const char *ptr, size_t len);
void freeStringObject(robj *o);
#endif