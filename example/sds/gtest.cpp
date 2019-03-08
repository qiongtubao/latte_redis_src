



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
    sdsfree(x);
}
TEST(testCase, test1) {
    sds x = sdsnewlen("foo", 2), y;
    ASSERT_STREQ("fo", x);
    sdsfree(x);
}

TEST(TestCase, test2) {
    sds x = sdsnew("foo");
    x = sdscat(x, "cat");
    ASSERT_STREQ("foocat", x);
    sdsfree(x);
}
TEST(TestCase, test3) {
    sds x = sdsnew("foo");
    x = sdscpy(x,"a");
    ASSERT_STREQ("a", x);
    sdsfree(x);
}
//赋值不同类型的sds 
TEST(TestCase, test4) {
    sds x = sdsnew("test4");
    x = sdscpy(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk");
    ASSERT_STREQ("xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk\0", x);
    sdsfree(x);
}
//格式化字符串
TEST(TestCase, test5) {
    sds x = sdsnew("test5");
    x = sdscatprintf(sdsempty(),"%d",123);
    ASSERT_STREQ("123\0", x);
    sdsfree(x);
}

//追加格式化字符串
TEST(TestCase, test6) {
    sds x = sdsnew("--");
    x = sdscatfmt(x, "Hello %s World %I,%I--", "Hi!", LLONG_MIN,LLONG_MAX);
    ASSERT_STREQ("--Hello Hi! World -9223372036854775808,"
                     "9223372036854775807--", x);
    sdsfree(x);
}
//去空
TEST(TestCase, test7) {
    sds x = sdsnew(" xy ");
    sdstrim(x," x");
    ASSERT_STREQ("y", x);
    sdsfree(x);
}
//截取
TEST(TestCase, test8) {
    sds x = sdsnew("xhelloy");
    sds y = sdsdup(x);
    sdsrange(y,1,1);
    ASSERT_STREQ("h", y);
    sdsfree(x);
    sdsfree(y);
}

TEST(TestCase, test9) {
    sds x = sdsnew("foo");
    sds y = sdsnew("foa");
    ASSERT_TRUE(sdscmp(x,y) > 0);
    sdsfree(x);
    sdsfree(y);
}
TEST(TestCase, test10) {
    sds x = sdsnewlen("\a\n\0foo\r",7);
    sds y = sdscatrepr(sdsempty(),x,sdslen(x));
    ASSERT_STREQ("\"\\a\\n\\x00foo\\r\"", y);
    sdsfree(x);
    sdsfree(y);
}

TEST(TestCase, test11) {
    sds x = sdsnew("hello");
    sdstoupper(x);
    ASSERT_STREQ("HELLO", x);
    sdsfree(x);
}

TEST(TestCase, test12) {
    sds x = sdsnew("HELLO");
    sdstolower(x);
    ASSERT_STREQ("hello", x);
    sdsfree(x);
}

TEST(TestCase, test13) {
    char *a[] = {const_cast<char*>("China"),const_cast<char*>("French"),const_cast<char*>("America"),const_cast<char*>("German")};
    char **s = a;
    sds x = sdsjoin(s, 4, const_cast<char*>(",,"));
    ASSERT_STREQ("China,,French,,America,,German", x);
    sdsfree(x);
}

int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









