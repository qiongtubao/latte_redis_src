
#ifndef __RSTRING_H
#define __RSTRING_H

#include "../object.h"
#include <stdio.h>


robj *createStringObject(const char *ptr, size_t len);

#endif