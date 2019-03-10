



#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gtest/gtest.h>
#ifdef __cplusplus 
extern "C" {
#endif
#include "./adlist.h"
#ifdef __cplusplus 
}
#endif




TEST(testCase, test0)
{
    list* list = listCreate();
    for(int i = 0; i < 10; i++) {
        listAddNodeHead(list, (void*)i);
    }
    listIter  *iter = listGetIterator(list, 0);
    int size = 0;
    listNode *node;
    while((node = listNext(iter)) != NULL) {
        size++;
    }
    ASSERT_TRUE(size ==  10);
}

int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









