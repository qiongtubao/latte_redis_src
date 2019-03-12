



#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gtest/gtest.h>
#ifdef __cplusplus 
extern "C" {
#endif
#include "./intset.h"
#ifdef __cplusplus 
}
#endif




TEST(testCase, test0)
{
    intset* it = intsetNew();
    uint8_t success;
    intsetAdd(it, 1, &success);
    ASSERT_TRUE(success == 1);
}

TEST(testCase, test1) {
    ASSERT_TRUE(intsetValueEncoding(-32768) == INTSET_ENC_INT16);
    ASSERT_TRUE(intsetValueEncoding(+32767) == INTSET_ENC_INT16);
    ASSERT_TRUE(intsetValueEncoding(-32769) == INTSET_ENC_INT32);
    ASSERT_TRUE(intsetValueEncoding(+32768) == INTSET_ENC_INT32);
    ASSERT_TRUE(intsetValueEncoding(-2147483648) == INTSET_ENC_INT32);
    ASSERT_TRUE(intsetValueEncoding(+2147483647) == INTSET_ENC_INT32);
    ASSERT_TRUE(intsetValueEncoding(-2147483649) == INTSET_ENC_INT64);
    ASSERT_TRUE(intsetValueEncoding(+2147483648) == INTSET_ENC_INT64);
    ASSERT_TRUE(intsetValueEncoding(-9223372036854775808ull) ==
                INTSET_ENC_INT64);
    ASSERT_TRUE(intsetValueEncoding(+9223372036854775807ull) ==
                INTSET_ENC_INT64);
}

TEST(testCase, test2) {
    intset* is = intsetNew();
    is = intsetAdd(is,32,NULL);
    ASSERT_TRUE(intrev32ifbe(is->encoding) == INTSET_ENC_INT16);
    is = intsetAdd(is,65535,NULL);
    ASSERT_TRUE(intrev32ifbe(is->encoding) == INTSET_ENC_INT32);
    ASSERT_TRUE(intrev32ifbe(is->length) == 2);
    ASSERT_TRUE(intsetFind(is,32) == 0);
}

int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









