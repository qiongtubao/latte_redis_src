



#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gtest/gtest.h>
#ifdef __cplusplus 
extern "C" {
#endif
#include "./sds.h"
#ifdef __cplusplus 
}
#endif



TEST(testCase, test0)
{
    sds x = sdsnew("foo"), y;
    ASSERT_STREQ("foo", x);
}
TEST(testCase, test1) {
    sds x = sdsnewlen("foo", 2), y;
    ASSERT_STREQ("fo", x);
}

TEST(TestCase, test2) {
    sds x = sdsnew("foo");
    x = sdscat(x, "cat");
    ASSERT_STREQ("foocat", x);
}
int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









