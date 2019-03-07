

#include <stdio.h>
#include <string.h>
#ifdef __cplusplus 
extern "C" {
#endif
    #include "./sds.h"
#ifdef __cplusplus 
}
#endif

void testSdsNewLen() {
    sds x = sdsnewlen("foo", 2), y;
    if(memcmp(x,"foo\0",4) == 0) {
        printf("sds4: %s",x);
    }else{
        printf("sds: %s",x);
    }
}
void testSdsNew() {
    sds x = sdsnew("foo"), y;
    if(memcmp(x,"foo\0",4) == 0) {
        printf("sds4: %s",x);
    }else{
        printf("sds: %s",x);
    }
}
int main(void) {
    testSdsNewLen();
    return 1;
}










// typedef struct data_type{
//     int age;
//     char name[20];
// }data;
// int main(void) {
//     data* bob=NULL;
//     bob=(data*)zmalloc(sizeof(data));
//     if(bob!=NULL)
//     {
//         bob->age=22;
//         strcpy(bob->name,"Robert"); 
//         printf("%s is %d years old.\n",bob->name,bob->age);
//     }
//     else
//     {
//         printf("mallocerror!\n");
//         return -1;
//     }  
//     zfree(bob);
//     return 1;
// }

