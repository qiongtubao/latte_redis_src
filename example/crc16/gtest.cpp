
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gtest/gtest.h>
#ifdef __cplusplus 
extern "C" {
#endif
#include "./crc16.h"
#ifdef __cplusplus 
}
#endif

TEST(testCase, test0)
{
    char str[8];
    sprintf(str, "%d",  crc16("123456789",9));
    ASSERT_STREQ("12739", str);
}


int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}