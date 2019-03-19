#include <stdio.h>
#include <string.h>
#ifdef __cplusplus 
extern "C" {
#endif
#include "./rstring.h"
// #include "../db.h"
// #include "../dict.h"
// #include <zmalloc.h>
// #include <redisutil.h>
#include "../mdb.h"
#ifdef __cplusplus 
}
#endif



int main(int argc, char * argv[]) {
    redisDb* db = createDb(2);
    robj* key = createStringObject("Hello", 5);
    robj* value = createStringObject("world",5);
    // printf("%lu", sizeof(struct sdshdr5));
    dbAdd(db, key, value);
    return 0;
}









