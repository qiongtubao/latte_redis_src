
#include "db.h"
#include "../dict/dict.h"
#include <zmalloc.h>
#include <redisutil.h>

redisDb* createDb(int dbnum);
