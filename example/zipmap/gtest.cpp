












#include <stdio.h>
#include <string.h>
#include <iostream>
#include <gtest/gtest.h>
#include <string>
using namespace std;
#ifdef __cplusplus 
extern "C" {
#endif
#include "./zipmap.h"
#include <zmalloc.h>
#ifdef __cplusplus 
}
#endif


void coutMap(unsigned char* zm) {
    int i = 0;
    while(zm[i] != ZIPMAP_END) {
        cout << hex << (unsigned int)zm[i] <<" ";
        i++;
    }
    cout << "\n";
    i = 0;
    while(zm[i]  != ZIPMAP_END) {
        for (int j=7;j>=0;j--)  {
            cout << ((zm[i] >> j) & 1);    
        }
        cout << " ";
        i++;
    }
    cout << "\n";
    unsigned char *index = zipmapRewind(zm);
    unsigned char *key, *value;
    unsigned int klen, vlen;
    cout << "kv:" << endl;
    while((index = zipmapNext(index,&key,&klen,&value,&vlen)) != NULL) {
       printf("  %d:%.*s => %d:%.*s\n", klen, klen, key, vlen, vlen, value);
    }
    cout << "\n";
}




/*
 * 1      4         [6e 61 6d 65]    5        0       [48 65 6c 6c 6f]    
 * 1个kv  4个字节       name          5个字节   分割符      Hello            
 */

TEST(testCase, test0)
{
    unsigned char *zm = zipmapNew();

    zm = zipmapSet(zm,(unsigned char*) "name",4, (unsigned char*) "Hello",5,NULL);
    coutMap(zm);
    zm = zipmapSet(zm, (unsigned char*) "name", 4, (unsigned char*) "aaaaaaaaaaaaaaaaaaaaaaa", 20, NULL);
    coutMap(zm);
    ASSERT_EQ(zipmapExists(zm, (unsigned char*) "name", 4), 1);
    ASSERT_EQ(zipmapLen(zm), 1);
    zipmapDel(zm,(unsigned char*) "name", 4, NULL);
    ASSERT_EQ(zipmapLen(zm), 0);
    coutMap(zm);
    zfree(zm);
}




int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









