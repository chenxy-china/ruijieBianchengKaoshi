#include <stdio.h>
#include <string.h>

int main()
{
    char  src_str[256]={0,};
    char* dest_str = "source char string";
    printf("test %ld, %ld ; %ld, %ld \n",sizeof(src_str) ,strlen(src_str), sizeof(dest_str) ,strlen(dest_str));
    //test 256, 0 ; 8, 18

    return 0;
}

