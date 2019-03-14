
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gtest/gtest.h>
#ifdef __cplusplus 
extern "C" {
#endif
#include "./crc64.h"
#ifdef __cplusplus 
}
#endif

TEST(testCase, test0)
{
    char str[17];
    sprintf(str, "%016llx", (unsigned long long) crc64(0,(unsigned char*)"123456789",9));
    ASSERT_STREQ("e9c6d914c4b8d9ca", str);
}


int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}