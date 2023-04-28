
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>

int chk_regular(char *pattern, char *str)
{
    int find = 1;
    regex_t reg;
    int rtn = regcomp(&reg,pattern,REG_ICASE|REG_EXTENDED);
    if(rtn)
    {
        fprintf(stderr,"compile failed.\n");
        return -1;
    }
    regmatch_t pmatch[10];
    int nmatch=10;

    rtn = regexec(&reg,str,nmatch,&pmatch[0],0);
    if(rtn == REG_NOMATCH)
    {
        find = 0;
    }
    regfree(&reg);

    return find;
}

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

    //如果中间含有空格
    if(strstr(str," ") != NULL){
        return 0;
    }

    //只能有一个的字符"e",".","+","-"
    char onlyonechar[]="e.+-";
    i=0;
    while(onlyonechar[i] != '\0'){
        if(strchr(str,onlyonechar[i]) != strrchr(str,onlyonechar[i])){
            return 0;
        }
        i++;
    }


    int result = 1;
    //如果不含有"e"
    if(strchr(str,'e') == NULL){

        //如果含有"+","-"但是不在开头
        if(((strchr(str,'+') != NULL ) && (strchr(str,'+') != str )) ||
        ((strchr(str,'-') != NULL ) && (strchr(str,'-') != str )) ){
            return 0;
        }

        //如果只含有一个符号
        if((strcmp(str,"+") == 0) || (strcmp(str,"-") == 0) || (strcmp(str,".") == 0)){
            result = 0;
        }

        //如果含有字母
        if(chk_regular("[a-z]+",str) == 1){
            result = 0;
        }

        //如果全是数字
        if(chk_regular("^[+-]?[^a-zA-Z][0-9]*[.]?[0-9]*",str) != 1){
            result = 0;
        }

        return result;
    }else{
        //如果含有"e"

        //如果"e"在开头
        if((strchr(str,'e') == str)){
            return 0;
        }

        //正则表达式
        char *numpattern1="^[+-]?[^a-z][0-9]*[.]?[0-9]*";
        char *numpattern2="^[+-]?[0-9]+";
        char *numpattern3="[a-z]+";
        regex_t reg1,reg2,reg3;

        int rtn = regcomp(&reg1,numpattern1,REG_ICASE|REG_EXTENDED);
        if(rtn)
        {
            fprintf(stderr,"compile failed.\n");
            return -1;
        }
        rtn = regcomp(&reg2,numpattern2,REG_ICASE|REG_EXTENDED);
        if(rtn)
        {
            fprintf(stderr,"compile failed.\n");
            regfree(&reg1);
            return -1;
        }
        rtn = regcomp(&reg3,numpattern3,REG_ICASE|REG_EXTENDED);
        if(rtn)
        {
            fprintf(stderr,"compile failed.\n");
            regfree(&reg1);
            regfree(&reg2);
            return -1;
        }
        regmatch_t pmatch[10];
        int nmatch=10;
        
        //用字符e分割
        const char s[2]="e";
        char *token;

        token = strtok(str,s);
        if(token != NULL){
            //printf("%s\n",token);

            //如果含有"+","-"但是不在开头
            if(((strchr(token,'+') != NULL ) && (strchr(token,'+') != token )) ||
            ((strchr(token,'-') != NULL ) && (strchr(token,'-') != token )) ){
                result = 0;
            }

            //如果只含有一个符号
            if((strcmp(token,"+") == 0) || (strcmp(token,"-") == 0) || (strcmp(token,".") == 0)){
                result = 0;
            }

            //正则表达式判断
            //如果含有字母
            rtn = regexec(&reg3,token,nmatch,&pmatch[0],0);
            if(rtn != REG_NOMATCH)
            {
                result = 0;
            }

            //不含数字
            rtn = regexec(&reg1,token,nmatch,&pmatch[0],0);
            if(rtn == REG_NOMATCH)
            {
                result = 0;
            }else{
                printf("token1 %s\n",&token[pmatch[0].rm_so]);
            }
        }else{
            printf("token1 null\n");
            result = 0;
        }
        regfree(&reg1);
        regfree(&reg3);
        
        if(result == 1){
            token = strtok(NULL, s);
            if(token != NULL){

                //如果含有"."
                if(strchr(token,'.') != NULL){
                    printf("token 2 have .\n");
                    result = 0;
                }

                //如果含有"+","-"但是不在开头
                if(((strchr(token,'+') != NULL ) && (strchr(token,'+') != token )) ||
                ((strchr(token,'-') != NULL ) && (strchr(token,'-') != token )) ){
                    result = 0;
                }


                //如果只含有一个符号
                if((strcmp(token,"+") == 0) || (strcmp(token,"-") == 0) ){
                    result = 0;
                }

                //正则表达式判断
                rtn = regexec(&reg2,token,nmatch,&pmatch[0],0);
                if(rtn == REG_NOMATCH)
                {
                    result = 0;
                }else{
                    printf("token2 %s\n",&token[pmatch[0].rm_so]);
                }
            }else{
                printf("token2 null\n");
                result = 0;
            }
        }
        regfree(&reg2);
        return result;
    }
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