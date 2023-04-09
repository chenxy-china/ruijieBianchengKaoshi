//先实现遍历目录功能

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

int recur_cnt = 0;  //递归次数，就是目录深度
int scanFile(char *szDir)
{
    struct dirent **entry_list;
    int count;
    int i;
 
    recur_cnt++;
    count = scandir(szDir, &entry_list, 0, alphasort);
    if (count < 0) {
        fprintf(stderr,"scandir [%s] error,%s\n",szDir,strerror(errno));
        return EXIT_FAILURE;
    }
    for (i = 0; i < count; i++) {
        struct dirent *entry;
        entry = entry_list[i];

        if(entry->d_type & DT_DIR){
            //目录
            if (strcmp(entry->d_name, ".") == 0 
                || strcmp(entry->d_name, "..") == 0) continue;

            //按递归次数(目录深度)打印空格数量
            for(int i = 0; i < recur_cnt; i++){
                printf("  ");
            }
            //目录输出目录名
            printf("-%s\n", entry->d_name);

            //从一个当前目录名，变成绝对路径
            char Path[256];
            int len = strlen(szDir);
            strncpy(Path, szDir, len + 1);
            if (szDir[len - 1] != '/') 
                strncat(Path, "/", 2);
            strncat(Path, entry->d_name, strlen(entry->d_name) + 1);

            //递归调用
            if(scanFile(Path) < 0){
                printf("递归调用错误\n");
            }
        }else{
            //按递归次数(目录深度)打印空格数量
            for(int i = 0; i < recur_cnt; i++){
                printf("  ");
            }
            //非目录输出文件名
            printf("-%s\n", entry->d_name);
        }
        free(entry);
    }
    free(entry_list);
 
    recur_cnt--;
    return 0;
}

int main(int argc,char** argv)
{
    char * dir = "/home/chenxy/test";
    //输出根目录
    printf("%s\n",dir);
    scanFile(dir);
}