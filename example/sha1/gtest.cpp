












#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gtest/gtest.h>
#include <string>
using namespace std;
#ifdef __cplusplus 
extern "C" {
#endif
#include "./sha1.h"
#ifdef __cplusplus 
}
#endif

#define BUFSIZE 4096
char* toSha1(const char* buf) {
    SHA1_CTX ctx;
    unsigned char hash[20];
    int i;
    SHA1Init(&ctx);
    SHA1Update(&ctx, (const unsigned char *)buf,strlen(buf));
    SHA1Final(hash, &ctx);
    char str[BUFSIZE] = {0};
    int len = 0;
    for(i=0;i<20;i++)
        len += sprintf(str + len, "%02x", hash[i]);
    return str;
}

TEST(testCase, test0)
{
    ASSERT_STREQ(toSha1("abc "), "eeca658f56ce50a4f36605aaf93e29e87c12009a");
}

TEST(testCase, test1) {
    ASSERT_STREQ(toSha1("abc") , "a9993e364706816aba3e25717850c26c9cd0d89d");
}


int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









