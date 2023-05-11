#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>

void rm_other_blank(char *str)
{
    int i;
    int j;
    int is_first_blank;
    i = 0;j = 0;is_first_blank = 1;
    while (str[i] != '\0')  {
        if (str[i] != ' ')  {
            break;
        }
        i++;
    }
    while (str[i] != '\0')  {
        if (str[i] != ' ')  {
            str[j] = str[i];
            is_first_blank = 1;
            j++;
        }
        else  {
            if (is_first_blank == 1) {
                str[j] = str[i];
                is_first_blank = 0;
                j++;
            }
        }
        i++;
    }
    if (str[j - 1] == ' ') {
        str[j - 1] = '\0';
    }
    else {
        str[j] = '\0';
    }
}
int singlePidAnalysis(int pid)
{
    char pidDir[32],cmdFile[64],cmd[1024],uid[32],statusFile[64],line[1024],sectionAddress[32],
            RSS[32],statusSmaps[64],fdDir[64],fdFullname[512],linkfile[1024],lastline[1024],s[2];
    ssize_t len; FILE *fd; DIR *dir;
    struct dirent *pFile; char *token;
    memset(pidDir,0,sizeof(pidDir));
    memset(cmdFile,0,sizeof(cmdFile));
    memset(cmd,0,sizeof(cmd));
    memset(uid,0,sizeof(uid));
    memset(statusFile,0,sizeof(statusFile));
    memset(line,0,sizeof(line));
    memset(sectionAddress,0,sizeof(sectionAddress));
    memset(RSS,0,sizeof(RSS));
    memset(statusSmaps,0,sizeof(statusSmaps));
    pFile = NULL;
    memset(fdDir,0,sizeof(fdDir));
    memset(fdFullname,0,sizeof(fdFullname));
    memset(linkfile,0,sizeof(linkfile));
    memset(lastline,0,sizeof(lastline));
    token = NULL;
    len = 0;
    sprintf(pidDir, "/proc/%d", pid);
    if ((access(pidDir, F_OK)) == -1) {
        printf("pid %d is not exist\n", pid);
        return -1;
    }
    sprintf(cmdFile, "%s/exe", pidDir);
    if ((len = readlink(cmdFile, cmd, sizeof(cmd) - 1)) != -1) {
        cmd[len] = '\0';
    }
    printf("cmd line: \n  >> %s\n", cmd);
    sprintf(statusFile, "%s/status", pidDir);
    fd = fopen(statusFile, "r");
    if (fd == NULL)  {
        printf("open %s failed\n", statusFile); return -1;
    }
    memset(line, 0, sizeof(line));
    while (fgets(line, sizeof(line), fd) != NULL) {
        if (strstr(line, "Uid:") == line) {
            memset(s,0,sizeof(s));
            strcpy(s,"\t");
            strtok(line, s);
            token = strtok(NULL, s);
            if (token != NULL) {
                snprintf(uid, sizeof(uid) - 1, "%s\n", token);
            }
            printf("user id :%s \n", uid);
        }
    }
    fclose(fd);
    sprintf(statusSmaps, "%s/smaps", pidDir);
    fd = fopen(statusSmaps, "r");
    if (fd == NULL) {
        printf("open %s failed\n", statusSmaps); return -1;
    }
    memset(line, 0, sizeof(line));
    printf("Sections\t\tRSS\n");
    while (fgets(line, sizeof(line), fd) != NULL) {
        if (strstr(line, "Size:") == line) {
            if (strlen(lastline) != 0) {
                memset(s,0,sizeof(s));
                strcpy(s,"-");
                token = strtok(lastline, s);
            }
            if (token != NULL)  {
                snprintf(sectionAddress, sizeof(sectionAddress) - 1, "%s", token);
            }
        }
        if (strstr(line, "Rss:") == line) {
            rm_other_blank(line);
            memset(s,0,sizeof(s));
            strcpy(s," ");
            strtok(line, s);
            token = strtok(NULL, s);
            if (token != NULL) {
                snprintf(RSS, sizeof(RSS) - 1, "%s", token);
            }
        }
        if ((strlen(sectionAddress) != 0) && (strlen(RSS) != 0))  {
            printf("%s\t\t%s\n", sectionAddress, RSS);
            memset(sectionAddress, 0, sizeof(sectionAddress));
            memset(RSS, 0, sizeof(RSS));
        }
        strcpy(lastline, line);
    }
    fclose(fd);
    sprintf(fdDir, "%s/fd", pidDir);
    dir = opendir(fdDir);
    if (dir == NULL){
        printf("opendir is NULL\n"); return -1;
    }
    printf("fd\ttype\tfn\n");
    while ((pFile = readdir(dir)) != NULL){
        if (pFile->d_type & DT_DIR)  {
            if (strcmp(pFile->d_name, ".") == 0 || strcmp(pFile->d_name, "..") == 0){
                continue;
            }
        }
        sprintf(fdFullname, "%s/%s", fdDir, pFile->d_name);
        if ((len = readlink(fdFullname, linkfile, sizeof(linkfile) - 1)) != -1) {
            linkfile[len] = '\0';
        }
        if (strlen(linkfile) != 0) {
            printf("%s\t", pFile->d_name);
            if (strstr(linkfile, "/dev") != NULL) {
                printf("device\t");
            } else if (strstr(linkfile, "socket") != NULL)  {
                printf("socket\t");
            } else if (strstr(linkfile, "pipe") != NULL)  {
                printf("FIFO/pipe\t");
            } else if (strstr(linkfile, "/") == linkfile)  {
                printf("regular_file\t");
            }else {
                printf("unknown?\t");
            }
            printf("%s\n", linkfile);
        }
    }
    closedir(dir);
    return 0;
}
int dirSelect(const struct dirent *dir)
{
    int i;
    i = -1;
    if (dir->d_type & DT_DIR)  {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
            return 0;
        }
        i = atoi(dir->d_name);
        if (i == 0) {
            return 0;
        } else {
            return 1;
        }
    }
    return 0;
}
char resourcename[16] = "VmSize";
int getResourceValue(const char *pid)
{
    char rvstr[16];
    int rv;
    FILE *fd;
    char statusFile[512];
    char line[128];
    char s[2];
    char *token;

    memset(rvstr,0,sizeof(rvstr));
    memset(statusFile,0,sizeof(statusFile));
    rv = 0;
    memset(line,0,sizeof(line));

    sprintf(statusFile, "/proc/%s/status", pid);
    fd = fopen(statusFile, "r");
    if (fd == NULL) {
        printf("open %s failed\n", statusFile);
        return -1;
    }

    memset(line, 0, sizeof(line));
    while (fgets(line, sizeof(line), fd) != NULL) {
        if (strstr(line, resourcename) == line) {
            memset(s,0,sizeof(s));
            strcpy(s,"\t");
            strtok(line, s);
            token = strtok(NULL, s);
            if (token != NULL)  {
                snprintf(rvstr, sizeof(rvstr) - 1, "%s\n", token);
                rv = atoi(rvstr);
            }
        }
    }
    fclose(fd);
    return rv;
}
int dirCompar(const struct dirent **dir1, const struct dirent **dir2)
{
    if (getResourceValue((*dir1)->d_name) > getResourceValue((*dir2)->d_name)) {
        return 1;
    }
    return 0;
}
int sortAllProc(void)
{
    struct dirent **entry_list;
    int count;
    count = scandir("/proc/", &entry_list, dirSelect, dirCompar);
    if (count < 0) {
        fprintf(stderr, "scandir /proc error,%s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    printf("----%s sort----\n", resourcename);
    for (int i = 0; i < count; i++) {
        struct dirent *entry;
        entry = entry_list[i];
        printf("%s\t%d\n", entry->d_name, getResourceValue(entry->d_name));
        free(entry);
    }
    free(entry_list);
    return 0;
}
int main(int argc, char **argv)
{
    int pid;
    pid = -1;
    if (argc != 2) {
        printf("Usage: %s [Pid | ResouceName] \n", argv[0]);
        printf("argc %d ,argv :%s  \n", argc, argv[0]);
        return 0;
    }
    pid = atoi(argv[1]);
    if (pid != 0) {
        singlePidAnalysis(pid);
    }
    else {
        strcpy(resourcename, argv[1]);
        resourcename[strlen(argv[1])] = '\0';
        sortAllProc();
    }
    return 0;
}
