#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void rm_other_blank(char *str)
{
    int i = 0;
    int j = 0;
    int is_first_blank = 1;

    while (str[i] != '\0') // 处理字符串开头为空格
    {
        if (str[i] != ' ')
        {
            break;
        }
        i++;
    }

    while (str[i] != '\0') // 处理字符产中间为空格
    {
        if (str[i] != ' ')
        {
            str[j] = str[i];
            is_first_blank = 1;
            j++;
        }
        else
        {
            if (is_first_blank == 1)
            {
                str[j] = str[i];
                is_first_blank = 0;
                j++;
            }
        }
        i++;
    }

    if (str[j - 1] == ' ') // 处理字符串末尾的空格：全部删掉。
    {
        str[j - 1] = '\0';
    }
    else
    {
        str[j] = '\0';
    }
}

int singlePidAnalysis(int pid)
{
    char pidDir[32] = {
        0,
    };
    sprintf(pidDir, "/proc/%d", pid);

    if ((access(pidDir, F_OK)) == -1)
    {
        printf("pid %d is not exist\n", pid);
        return -1;
    }

    // 获取进程程序对应的命令行
    char cmdFile[64] = {
        0,
    };
    sprintf(cmdFile, "%s/exe", pidDir);

    char cmd[1024] = {
        0,
    };
    ssize_t len = 0;
    if ((len = readlink(cmdFile, cmd, sizeof(cmd) - 1)) != -1)
    {
        cmd[len] = '\0';
    }
    printf("cmd line: \n  >> %s\n", cmd);
    printf("===================================\n");
    
    // 进程所属的用户Id
    // 进程的用户Id，可以从/pro/<PID>/status中读取，其中的Uid字段为用户Id
    char uid[32] = {
        0,
    };
    char statusFile[64] = {
        0,
    };
    sprintf(statusFile, "%s/status", pidDir);
    FILE *fd;
    fd = fopen(statusFile, "r");
    if (fd == NULL)
    {
        printf("open %s failed\n", statusFile);
        return -1;
    }

    char line[1024] = {
        0,
    };
    memset(line, 0, sizeof(line));
    while (fgets(line, sizeof(line), fd) != NULL)
    {
        // 查找Uid：开头的字符串
        if (strstr(line, "Uid:") == line)
        {
            // 获取第一个空格后的
            const char s[2] = "\t";
            char *token;
            token = strtok(line, s);
            /* 继续获取其他的子字符串 */
            token = strtok(NULL, s);
            if (token != NULL)
            {
                snprintf(uid, sizeof(uid) - 1, "%s\n", token);
            }
            printf("user id :%s \n", uid);
        }
    }
    fclose(fd);
    printf("===================================\n");

    // 对物理内存资源的使用，要求精确到各section内的占用情况
    // 进程的内存分布情况，可以从/pro/<PID>/smaps中读取，其中的RSS字段为物理内存占用
    char sectionAddress[32] = {
        0,
    };
    char RSS[32] = {
        0,
    };
    char statusSmaps[64] = {
        0,
    };
    sprintf(statusSmaps, "%s/smaps", pidDir);

    fd = fopen(statusSmaps, "r");
    if (fd == NULL)
    {
        printf("open %s failed\n", statusSmaps);
        return -1;
    }

    char lastline[1024] = {
        0,
    };
    memset(line, 0, sizeof(line));
    printf("Sections\t\tRSS\n");
    while (fgets(line, sizeof(line), fd) != NULL)
    {
        // 查找Size:开头的字符串
        if (strstr(line, "Size:") == line)
        {
            // 获取前一行 ，第一个"-"前的数据
            const char s[2] = "-";
            char *token;
            if (strlen(lastline) != 0)
            {
                token = strtok(lastline, s);
            }

            if (token != NULL)
            {
                snprintf(sectionAddress, sizeof(sectionAddress) - 1, "%s", token);
            }
        }
        // 查找Rss:开头的字符串
        if (strstr(line, "Rss:") == line)
        {
            //将这一行中间的多个空格转换成一个
            rm_other_blank(line);
            // 获取" "后的数据
            const char s[2] = " ";
            char *token;
            token = strtok(line, s);
            /* 继续获取其他的子字符串 */
            token = strtok(NULL, s);
            if (token != NULL)
            {
                snprintf(RSS, sizeof(RSS) - 1, "%s", token);
            }
        }

        if ((strlen(sectionAddress) != 0) && (strlen(RSS) != 0))
        {
            printf("%s\t\t%s\n", sectionAddress, RSS);
            memset(sectionAddress, 0, sizeof(sectionAddress));
            memset(RSS, 0, sizeof(RSS));
        }

        strcpy(lastline, line); // 保留上一行
    }
    fclose(fd);
    printf("===================================\n");

    // 文件句柄资源，要求区分文件类型，如常规文件、设备文件和socket等
    char fdDir[64] = {
        0,
    };
#if 1
    sprintf(fdDir, "%s/fd", pidDir);

    DIR   *dir;
    struct  dirent *pFile = NULL ;
    dir = opendir(fdDir);
    if(NULL == dir)
    {
        printf("opendir is NULL\n");
        return -1;
    }

    char fdFullname[512]={0,};
    char linkfile[1024];
    printf("fd\ttype\tfn\n");
    while( (pFile = readdir(dir)) != NULL)
    {
        if (pFile->d_type & DT_DIR) {
            if (strcmp(pFile->d_name, ".") == 0 
                || strcmp(pFile->d_name, "..") == 0) continue;
        }

        sprintf(fdFullname,"%s/%s",fdDir,pFile->d_name);
        if ((len = readlink(fdFullname, linkfile, sizeof(linkfile) - 1)) != -1) {
            linkfile[len] = '\0';
        }

        if(strlen(linkfile) != 0){
            printf("%s\t",pFile->d_name);
            
            if (strstr(linkfile,"/dev") != NULL){
                printf("device\t");
            }else if (strstr(linkfile,"socket") != NULL){
                printf("socket\t");
            }else if (strstr(linkfile,"pipe") != NULL){
                printf("FIFO/pipe\t");
            }else if (strstr(linkfile,"/") == linkfile){
                printf("regular_file\t");
            }else{
                printf("unknown?\t");
            }
            printf("%s\n",linkfile);
        }
    }
#else
    sprintf(fdDir, "%s/fdinfo", pidDir);

    DIR   *dir;
    struct  dirent *pFile = NULL ;
    dir = opendir(fdDir);
    if(NULL == dir)
    {
        printf("opendir is NULL\n");
        return -1;
    }

    int fdInt = -1;
    struct stat file_stat = {0,};
    printf("open file information\n");
    printf("--------------------------------------\n");
    printf("fd\ttype\tfn\n");
    while( (pFile = readdir(dir)) != NULL)
    {
        if (pFile->d_type & DT_DIR) {
            if (strcmp(pFile->d_name, ".") == 0 
                || strcmp(pFile->d_name, "..") == 0) continue;
        }

        fdInt = -1;
        fdInt = atoi(pFile->d_name);
        memset(&file_stat,0,sizeof(file_stat));
        fstat(fdInt, &file_stat);

        printf("%s\t",pFile->d_name);

        switch (file_stat.st_mode & S_IFMT) {
            case S_IFBLK:  printf("block device\n");            break;
            case S_IFCHR:  printf("character device\n");        break;
            case S_IFDIR:  printf("directory\n");               break;
            case S_IFIFO:  printf("FIFO/pipe\n");               break;
            case S_IFLNK:  printf("symlink\n");                 break;
            case S_IFREG:  printf("regular file\n");            break;
            case S_IFSOCK: printf("socket\n");                  break;
            default:       printf("unknown?\n");                break;
        }

    }
#endif
    closedir(dir);
    return 0;
}

int main(int argc, char **argv)
{
    int pid = 3326;
    if (argc == 2){
        pid = atoi(argv[1]);
        singlePidAnalysis(pid);

    }else{


    }
}