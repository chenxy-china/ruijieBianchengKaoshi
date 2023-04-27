
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>

int chk_number(char *str)
{

    int i = 0;
    // 处理字符串开头空格
    while (str[i] != '\0') 
    {
        if (str[i] != ' ')
        {
            break;
        }
        i++;
    }
    str = str+i;

    // 处理字符串尾巴空格
    i=strlen(str)-1;
    while (i != 0) 
    {
        if (str[i] != ' ')
        {
            break;
        }else
        {
            str[i] = '\0';
        }
        i--;
    }


    if(strstr(str," ") != NULL){
        //如果中间含有空格
        return 0;
    }

    const char s[2]="e";
    char *token;

    token = strtok(str,s);
    while(token != NULL){

        token = strtok(NULL, s);

        return 0;
    }
    return 1;
}

int main (int argc,char *argv[])
{   
    int rtn = -1;
    char buf[1024] = {0};

    while(1){
        if(NULL == fgets(buf, 1024, stdin)){
            return -1;
        }

        buf[strlen(buf)-1] = '\0';		// 去除\n

        if(strcmp(buf,"quit") == 0){
            break;
        }

        rtn = chk_number(buf);
        printf("==>%d\n",rtn);
    }

    return 0;
}