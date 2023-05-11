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
    strcpy(onlyonechar,"e");
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
    int result;
    unsigned char ch, lch;
    char buf[1024];
    char lbuf[1024];
    int i;
    fd_set readfds;
    int fd;
    int ret;

    i = 0;
    ch = '\0', lch = '\0';
    memset(buf,0,sizeof(buf));
    memset(lbuf,0,sizeof(lbuf));
    ret = -1;
    fd = STDIN_FILENO;
    struct termios new_setting, init_setting;
    tcgetattr(0, &init_setting);
    new_setting = init_setting;
    new_setting.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &new_setting);

    FD_ZERO(&readfds);
    FD_SET(0, &readfds);

    printf("please enter : \n");
    while (1) {
        pselect(1, &readfds, NULL, NULL, NULL, NULL);
        result = read(fd, &ch, 1);
        if (result != 1) {
            break;
        }
        lch = ch;
        if (ch == ARROWKEY) {
            continue;
        }
        else if (lch == ARROWKEY) {
            if (ch == 'A') {
                ret = write(STDOUT_FILENO, &lbuf, sizeof(lbuf));
                memcpy(buf, lbuf, sizeof(lbuf));
                i = sizeof(lbuf);
                if (ret == 0){
                    continue;
                }
            }
            continue;
        }
        else if (ch != '\n') {
            ret = write(STDOUT_FILENO, &ch, 1);
            buf[i] = ch;
            i++;
            if (ret == 0){
                continue;
            }
            continue; // avoid compile error
        }
        buf[i - 1] = '\0';
        memcpy(lbuf, buf, sizeof(buf));
        result = chk_number(buf);
        printf("==>%d\n", result);
        printf("please enter : \n");
        i = 0;
    } /* end while */
    tcsetattr(0, TCSANOW, &init_setting);
    return 0;
}
