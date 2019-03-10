



#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gtest/gtest.h>
#ifdef __cplusplus 
extern "C" {
#endif
#include "./dict.h"
#include "../sds/sds.h"
#ifdef __cplusplus 
}
#endif

uint64_t hashCallback(const void *key) {
    return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
}

int compareCallback(void *privdata, const void *key1, const void *key2) {
    int l1,l2;
    DICT_NOTUSED(privdata);

    l1 = sdslen((sds)key1);
    l2 = sdslen((sds)key2);
    if (l1 != l2) return 0;
    return memcmp(key1, key2, l1) == 0;
}

void freeCallback(void *privdata, void *val) {
    DICT_NOTUSED(privdata);
    sdsfree((char*)val);
}
dictType BenchmarkDictType = {
    hashCallback,
    NULL,
    NULL,
    compareCallback,
    freeCallback,
    NULL
};
TEST(testCase, test0)
{
    dict *dict = dictCreate(&BenchmarkDictType,NULL);
    int retval = dictAdd(dict,sdsfromlonglong(1),((void*)1) );
    ASSERT_EQ(retval, DICT_OK);
    ASSERT_TRUE(dictSize(dict) ==  1);
    zfree(dict);
}

TEST(testCase, test1)
{
    dict *dict = dictCreate(&BenchmarkDictType,NULL);
    int retval = dictAdd(dict,sdsfromlonglong(1),((void*)1) );
    // ASSERT_EQ(retval, DICT_OK);
    dictEntry* d =  dictFind(dict, sdsfromlonglong(1));
    ASSERT_FALSE(d == NULL);
    ASSERT_EQ(dictDelete(dict, sdsfromlonglong(1)), DICT_OK);
    ASSERT_TRUE(dictSize(dict) ==  0);
    zfree(dict);
}


TEST(testCase, test1)
{
    dict *dict = dictCreate(&BenchmarkDictType,NULL);
    int retval = dictAdd(dict,sdsfromlonglong(1),((void*)1) );
    // ASSERT_EQ(retval, DICT_OK);
    dictEntry* d =  dictFind(dict, sdsfromlonglong(1));
    ASSERT_FALSE(d == NULL);
    ASSERT_EQ(dictDelete(dict, sdsfromlonglong(1)), DICT_OK);
    ASSERT_TRUE(dictSize(dict) ==  0);
    zfree(dict);
}

int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









