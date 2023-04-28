
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>

int chk_regular(char *pattern, char *str,char full)
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
    if(rtn == REG_NOMATCH) {
        //没找到匹配项
        find = 0;
    } else if(rtn == REG_NOERROR) {
        //找到匹配项
        if(full == 1){
            //判断全匹配
            int len = pmatch[0].rm_eo - pmatch[0].rm_so;
            if(len != strlen(str)){
                find = 0;
            }
        }
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

        //正则表达式判断
        //如果含有字母
        if(chk_regular("[a-z]+",str,0) == 1){
            result = 0;
        }

        //如果不含有数字
        if(chk_regular("[0-9]+",str,0) == 0){
            result = 0;
        }

        //如果不全是数字
        if(chk_regular("^[+-]?[^a-zA-Z][0-9]*[.]?[0-9]*",str,1) != 1){
            result = 0;
        }

        return result;
    }else{
        //如果含有"e"

        //如果"e"在开头
        if((strchr(str,'e') == str)){
            return 0;
        }

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
            if(chk_regular("[a-z]+",token,0) == 1)
            {
                result = 0;
            }

            //如果不含有数字
            if(chk_regular("[0-9]+",token,0) == 0){
                result = 0;
            }

            //如果不全是数字
            if(chk_regular("^[+-]?[^a-z][0-9]*[.]?[0-9]*",token,1) != 1){
                result = 0;
            }

        }else{
            printf("token1 null\n");
            result = 0;
        }
        
        if(result == 1){
            token = strtok(NULL, s);
            if(token != NULL){

                //如果含有"."
                if(strchr(token,'.') != NULL){
                    //printf("token 2 have .\n");
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
                //如果含有字母
                if(chk_regular("[a-z]+",token,0) == 1)
                {
                    result = 0;
                }

                //如果不全是整数
                if(chk_regular("^[+-]?[0-9]+",token,1) != 1){
                    result = 0;
                }

            }else{
                //printf("token2 null\n");
                result = 0;
            }
        }
        return result;
    }
}

int main (int argc,char *argv[])
{   
    int rtn = -1;
    char buf[1024] = {0};
    char lbuf[1024] = {0};
    char ch = '\0';
    int i = 0;
    
    while(1)
    {
        ch = fgetc(stdin);
        if(ch == 0x1B ){
            break;  /* ESC 键退出 */
        }else if(ch == 0xE0 ){ //UP KEY 显示之前的字符
            ch = fgetc(stdin);
            if(ch == 0x48 ){
                printf("%s",lbuf);
            }
            continue;
        }else if( ch != '\n'){
            printf("%c",ch);
            buf[i] = ch;
            i++;
            continue;
        }

        memcpy(lbuf,buf,sizeof(buf));

        buf[i-1] = '\0';		// 去除\n
        rtn = chk_number(buf);
        printf("==>%d\n",rtn);

        i=0;
    }

    return 0;
}