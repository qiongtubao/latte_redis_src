












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
#ifdef __cplusplus 
}
#endif




TEST(testCase, test0)
{
    unsigned char *zm = zipmapNew();

    zm = zipmapSet(zm,(unsigned char*) "name",4, (unsigned char*) "foo",3,NULL);
    
}




int main(int argc, char * argv[]) {
    std::cout << "Hello, World!\n";
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}









