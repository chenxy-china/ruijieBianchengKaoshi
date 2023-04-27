#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>

int chk_line(char *line,const char *uri)
{
    int rtn=0,i=0,len=0;
    regex_t reg;
    regmatch_t pmatch[10];
    int nmatch=10;
    char *param;
    char *numpattern="([0-9]{1,})";
    char *strpattern="([A-Za-z_0-9]{1,})";
    const char s[2]="/";
    char *token;
    char pattern[512]={0,};

    fprintf(stderr,"pattern : %s\n",line);
    fprintf(stderr,"uri : %s\n",uri);

    //pattern line to regular expression
    //sprintf(cmd,"echo ^%s$|sed 's/#NUMBER#/[0-9]{1,}/' ",line);     
    //char *pattern = "^hisencyber(.com|.com.cn)";
    //char *pattern = "^/issue/[0-9]{1,}";
    //char *pattern = "^/report/([A-Za-z_0-9]{1,})/([A-Za-z_0-9]{1,})";
    //char *pattern = "^/report/([A-Za-z_0-9]{1,})/(show|delete|add|show)";

    token = strtok(line,s);
    while(token != NULL){
        fprintf(stderr,"child string : %s\n",token);
        if(*token == '#'){
            if(strncmp(token,"#NUMERIC#",9) == 0){
                i = snprintf(pattern + len, sizeof(pattern)-len, "/%s", numpattern);
                len = len + i;

            }else{
                i = snprintf(pattern + len, sizeof(pattern)-len, "/%s", strpattern);
                len = len + i;
            }
        }
        else{
            i = snprintf(pattern + len, sizeof(pattern)-len, "/%s", token);
            len = len + i;
        }
        token = strtok(NULL,s);
    }
    
    rtn = regcomp(&reg,pattern,REG_ICASE|REG_EXTENDED);
    if(rtn)
    {
         fprintf(stderr,"compile failed.\n");
         return -1;
    }

    rtn = regexec(&reg,uri,nmatch,&pmatch[0],0);
    if(rtn == 0)
    {
      i=0;
      while(1){
        i++;
        if(pmatch[i].rm_so == -1){
           break;
        }
        len = pmatch[i].rm_eo - pmatch[i].rm_so;
        param = (char*)malloc((len+1)*sizeof(char));
        memset(param,0,(len+1)*sizeof(char));
        memcpy(param,&uri[pmatch[i].rm_so],len);
        fprintf(stderr,"%s\n",param);
        free(param);
      }
    }

    regfree(&reg);
                                                                        
    fprintf(stderr,"\n");
    return 0;
}

int chk_file(const char *uri)
{
    FILE *fp;
    char line[256];

    fp = fopen("UriValPattern.cfg","r");
    if(fp == NULL)
    {
        fprintf(stderr,"Open file UriValPattern.cfg failed\n");
        return -1;
    }

    memset(line,0,sizeof(line));
    while(fgets(line,sizeof(line),fp)!= NULL)
    {
       chk_line(line,uri);
    }
    fclose(fp);
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
