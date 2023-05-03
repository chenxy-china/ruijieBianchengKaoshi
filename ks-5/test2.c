
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

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
    int result;
    unsigned char ch = '\0',lch = '\0';
    char buf[1024] = {0};
    char lbuf[1024] = {0};
    int i = 0;

    struct termios new_setting, init_setting;
    tcgetattr(0, &init_setting);
    new_setting = init_setting;    // get termios setting and save it
    new_setting.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &new_setting);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    int fd = STDIN_FILENO;

    printf("please enter : \n");
    while(1)
    {
        result = pselect (1, &readfds, NULL, NULL, NULL, NULL);

        result = read(fd, &ch, 1);

        if (result != 1) {
            break;
        }

        lch = ch;

        if(ch == 0x1B ){
            continue;
        }else if( lch == 0x1B){
            if(ch == 'A'){
                int ret = write(STDOUT_FILENO, &lbuf, sizeof(lbuf));
                memcpy(buf,lbuf,sizeof(lbuf));
                i=sizeof(lbuf);
                if(ret == 0)
                    continue;
            }
            continue;
        }else if( ch != '\n'){
            int ret = write(STDOUT_FILENO, &ch, 1);	
            buf[i] = ch;
            i++;
            if(ret == 0)
                continue;
            continue;
        }

        buf[i-1] = '\0';		// 去除\n
        memcpy(lbuf,buf,sizeof(buf));

        result = chk_number(buf);
        printf("==>%d\n",result);

        printf("please enter : \n");
        i=0;
    }

    tcsetattr(0, TCSANOW, &init_setting);
    return 0;
}