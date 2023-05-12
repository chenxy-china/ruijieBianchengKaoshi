#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

#define ARROWKEY 0x1B

int chk_regular(char *pattern, char *str, char full)
{
    int find;
    regex_t reg;
    regmatch_t pmatch[10];
    int nmatch;
    int len;
    int rtn;

    find = 1;
    nmatch = 10;
    len = 0;
    rtn = regcomp(&reg, pattern, REG_ICASE | REG_EXTENDED);
    if (rtn) {
        fprintf(stderr, "compile failed.\n");
        return -1;
    }

    rtn = regexec(&reg, str, nmatch, &pmatch[0], 0);
    if (rtn == REG_NOMATCH) {
        find = 0;
    }
    else if (rtn == REG_NOERROR) {
        if (full == 1) {
            len = pmatch[0].rm_eo - pmatch[0].rm_so;
            if (len != strlen(str)) {
                find = 0;
            }
        }
    }
    regfree(&reg);
    return find;
}
int chk_number(char *str)
{
    int i;
    char onlyonechar[5];
    int result;
    char s[2];
    char *token;

    i = 0;
    memset(onlyonechar,0,sizeof(onlyonechar));
    strcpy(onlyonechar,"e.+-");
    memset(s,0,sizeof(s));
    strcpy(s,"e");
    result = 1;
    while (str[i] != '\0') {
        if (str[i] != ' ') {
            break;
        }
        i++;
    }
    str = str + i;
    i = strlen(str) - 1;
    while (i != 0)  {
        if (str[i] != ' '){
            break;
        }
        else {
            str[i] = '\0';
        }
        i--;
    }
    if (strstr(str, " ") != NULL) {
        return 0;
    }

    i = 0;
    while (onlyonechar[i] != '\0'){
        if (strchr(str, onlyonechar[i]) != strrchr(str, onlyonechar[i])) {
            return 0;
        }
        i++;
    }

    if (strchr(str, 'e') == NULL) {
        if (((strchr(str, '+') != NULL) && (strchr(str, '+') != str)) 
                || ((strchr(str, '-') != NULL) && (strchr(str, '-') != str)))   {
            return 0;
        }
        if ((strcmp(str, "+") == 0) || (strcmp(str, "-") == 0) || (strcmp(str, ".") == 0)) {
            result = 0;
        }
        if (chk_regular("[a-z]+", str, 0) == 1) {
            result = 0;
        }
        if (chk_regular("[0-9]+", str, 0) == 0){
            result = 0;
        }
        if (chk_regular("^[+-]?[^a-zA-Z][0-9]*[.]?[0-9]*", str, 1) != 1) {
            result = 0;
        }
        return result;
    }
    else {
        if ((strchr(str, 'e') == str)) {
            return 0;
        }

        token = strtok(str, s);
        if (token != NULL) {
            if (((strchr(token, '+') != NULL) && (strchr(token, '+') != token))
                    || ((strchr(token, '-') != NULL) && (strchr(token, '-') != token)))  {
                result = 0;
            }
            if ((strcmp(token, "+") == 0) || (strcmp(token, "-") == 0) 
                    || (strcmp(token, ".") == 0)) {
                result = 0;
            }
            if (chk_regular("[a-z]+", token, 0) == 1) {
                result = 0;
            }
            if (chk_regular("[0-9]+", token, 0) == 0) {
                result = 0;
            }
            if (chk_regular("^[+-]?[^a-z][0-9]*[.]?[0-9]*", token, 1) != 1) {
                result = 0;
            }
        }
        else{
            printf("token1 null\n");
            result = 0;
        }
        if (result == 1) {
            token = strtok(NULL, s);
            if (token != NULL){
                if (strchr(token, '.') != NULL){
                    result = 0;
                }
                if (((strchr(token, '+') != NULL) && (strchr(token, '+') != token)) 
                        || ((strchr(token, '-') != NULL) && (strchr(token, '-') != token))) {
                    result = 0;
                }
                if ((strcmp(token, "+") == 0) || (strcmp(token, "-") == 0)){
                    result = 0;
                }
                if (chk_regular("[a-z]+", token, 0) == 1){
                    result = 0;
                }
                if (chk_regular("^[+-]?[0-9]+", token, 1) != 1) {
                    result = 0;
                }
            }
            else {
                result = 0;
            }
        }
        return result;
    } /* end if */
}
int main(int argc, char *argv[])
{
    int rtn;
    char buf[1024];
    memset(buf,0,sizeof(buf));
    
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
