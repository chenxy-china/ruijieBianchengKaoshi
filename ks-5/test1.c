
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>

int chk_line(int lineno, regex_t *reg,char *line)
{
    int rtn,len;
    regmatch_t pmatch;
    char *url,*pbuf;

    fprintf(stderr,"%4d:",lineno);
    rtn = regexec(reg,line,1,&pmatch,0);
    pbuf = line;
    while(rtn == 0)
    {
        len = pmatch.rm_eo - pmatch.rm_so;
        url = (char*)malloc((len+1)*sizeof(char));
        memset(url,0,(len+1)*sizeof(char));
        memcpy(url,&pbuf[pmatch.rm_so],len);
        fprintf(stderr,"%s",url);
        free(url);
        pbuf += pmatch.rm_eo;
        rtn = regexec(reg,pbuf,1,&pmatch,REG_NOTBOL);
    }
    fprintf(stderr,"\n");
    return 0;
}

int chk_file(const char *filename)
{
    FILE *fp;
    char *pattern = "^(hisencyber)(.com|.com.cn)";
    char line[1024];
    int rtn,lineno;
    regex_t reg;

    rtn = regcomp(&reg,pattern,REG_ICASE|REG_EXTENDED);
    if(rtn)
    {
        fprintf(stderr,"compile failed.\n");
        return -1;
    }

    fp = fopen(filename,"r");
    if(fp == NULL)
    {
        fprintf(stderr,"Open file %s failed\n",filename);
        return -1;
    }
    lineno = 1;
    memset(line,0,sizeof(line));
    while(fgets(line,sizeof(line),fp)!= NULL)
    {
        chk_line(lineno++,&reg,line);
    }
    fclose(fp);
    regfree(&reg);
    return 0;
}

int main (int argc,char *argv[])
{
    int rtn;
    if(argc != 2)
    {
        fprintf(stderr,"Usage:chkfileurl <file> \n");
        return 1;
    }
    rtn = chk_file(argv[1]);
    return rtn;
}