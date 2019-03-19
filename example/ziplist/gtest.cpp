

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gtest/gtest.h>
#include <string>
using namespace std;
#ifdef __cplusplus 
extern "C" {
#endif
#include <zmalloc.h>
#include "./ziplist.h"

#ifdef __cplusplus 
}
#endif
using namespace std;



/* _serverAssert is needed by dict */
void _serverAssert(const char *estr, const char *file, int line) {
    fprintf(stderr, "=== ASSERTION FAILED ===");
    fprintf(stderr, "==> %s:%d '%s' is not true",file,line,estr);
    *((char*)-1) = 'x';
}
void coutBit(unsigned char *zm) {
    uint32_t len = ZIPLIST_BYTES(zm);
    for(int i = 0; i < len; i++) {
        cout << hex << (unsigned int)zm[i] <<" ";
    }
    cout << "\n";
    for(int i = 0; i < len; i++) {
        for (int j=7;j>=0;j--)  {
            cout << ((zm[i] >> j) & 1);    
        }
        cout << " ";
    }
    cout << "\n";
}

TEST(testCase, test0)
{
    unsigned char *zm = ziplistNew();
    coutBit(zm);
    zm = ziplistPush(zm, (unsigned char*)"2", 1, ZIPLIST_TAIL); 
    printf("one: \n");
    coutBit(zm);
    zm = ziplistPush(zm, (unsigned char*)"5", 1, ZIPLIST_TAIL); 
    printf("two: \n");
    coutBit(zm);
    zm = ziplistPush(zm, (unsigned char*)"Hello world", 11, ZIPLIST_TAIL);
    printf("three: \n");
    coutBit(zm);
    
    unsigned char *p = ziplistIndex(zm, 2);
    unsigned char *vstr;
    unsigned int vlen;
    long long vlong;
    if(ziplistGet(p, &vstr,&vlen,&vlong)) {
        ASSERT_STREQ("Hello world\xFF" , (char*)vstr);
    }
    zm = ziplistDeleteRange(zm,0,1);
    coutBit(zm);
    zfree(zm);
}


TEST(testCase, test1) {
    unsigned char *zm = ziplistNew();
    coutBit(zm);
    zm = ziplistPush(zm, (unsigned char*)"2", 1, ZIPLIST_TAIL); 
    printf("one: \n");
    coutBit(zm);
    zm = ziplistPush(zm, (unsigned char*)"5", 1, ZIPLIST_TAIL); 
    printf("two: \n");
    coutBit(zm);
    unsigned char *zm2 = ziplistNew();
    zm2 = ziplistPush(zm2, (unsigned char*)"Hello world", 11, ZIPLIST_TAIL);
    printf("three: \n");
    zm = ziplistMerge(&zm, &zm2);
    coutBit(zm);
    zfree(zm);
}

TEST(testCase, test2) {
    unsigned char *zm = ziplistNew();
    zm = ziplistPush(zm, (unsigned char*)"2", 1, ZIPLIST_TAIL); 
    zm = ziplistPush(zm, (unsigned char*)"5", 1, ZIPLIST_TAIL); 
    zm = ziplistPush(zm, (unsigned char*)"Hello world", 11, ZIPLIST_TAIL);
    unsigned char *p = ziplistIndex(zm, 0);
    p = ziplistNext(zm, p);
    unsigned char *vstr;
    unsigned int vlen;
    long long vlong;
    if(ziplistGet(p, &vstr,&vlen,&vlong)) {
        ASSERT_EQ(vlong, 5);
    }
    p = ziplistPrev(zm, p);
    if(ziplistGet(p, &vstr,&vlen,&vlong)) {
        ASSERT_EQ(vlong, 2);
    }
    ASSERT_TRUE(ziplistLen(zm) == 3);
}




int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









