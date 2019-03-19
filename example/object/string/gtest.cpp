
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gtest/gtest.h>
#include <string>
using namespace std;
#ifdef __cplusplus 
extern "C" {
#endif
#include "./rstring.h"
#include "../mdb.h"
#include <zmalloc.h>
#ifdef __cplusplus 
}
#endif

redisDb* db = createDb(2);





/*
 * 1      4         [6e 61 6d 65]    5        0       [48 65 6c 6c 6f]    
 * 1个kv  4个字节       name          5个字节   分割符      Hello            
 */

TEST(testCase, test0)
{
    robj* key = createStringObject("Hello", 5);
    robj* value = createStringObject("world",5);
    // printf("%lu", sizeof(struct sdshdr5));
    dbAdd(db, key, value);
}




int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









