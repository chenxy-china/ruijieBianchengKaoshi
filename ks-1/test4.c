//先实现遍历目录功能，再保存到二叉树中

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

//#define OUTPUT1

//定义文件夹,文件的结构
struct Filenode{
    unsigned char type; //说明是文件，还是文件夹
    char name[256];     //文件夹的全路径名称 比如： rootdir/dir1/dia2/folder
    char result[24];    //比较的结果
    struct Filenode* child;     //子目录的指针
    struct Filenode* next;      //同目录的下一个文件夹、文件的指针

};

int recur_cnt = 0;  //递归次数，就是目录深度
int scanFile(struct Filenode *szDir)
{
    struct dirent **entry_list;
    int count;
    int i;

    //递归次数加1，就是目录深度加1
    recur_cnt++;

    if(NULL == szDir){
        perror("szDir is NULL");
        return -1;
    }
    count = scandir(szDir->name, &entry_list, 0, alphasort);
    if (count < 0) {
        fprintf(stderr,"scandir [%s] error,%s\n",szDir->name,strerror(errno));
        return EXIT_FAILURE;
    }

    struct Filenode *oldDir = NULL;
    for (i = 0; i < count; i++) {
        struct dirent *entry;
        entry = entry_list[i];

        if(entry->d_type & DT_DIR){
            //目录
            if (strcmp(entry->d_name, ".") == 0 
                || strcmp(entry->d_name, "..") == 0){
                continue;
            }

            //创建一个新的文件节点，将其窜到szDir->child节点或者next节点上
            struct Filenode *curDir = (struct Filenode *) calloc(sizeof(struct Filenode),1);
            if(i == 2){
                szDir->child = curDir;
            }else if(i > 2){
                if(NULL != oldDir){
                    oldDir->next = curDir;
                }else{
                    perror("oldDir is NULL");
                    //T.B.D 释放已经分配的curDir
                    return -1;
                }
            }
            oldDir = curDir;

            //从一个当前目录名，变成绝对路径
            int len = strlen(szDir->name);
            strncpy(curDir->name, szDir->name, sizeof(curDir->name));
            if (szDir->name[len - 1] != '/') 
                strncat(curDir->name, "/", 2);
            strcat(curDir->name, entry->d_name);
            curDir->type |= DT_DIR;

#ifdef OUTPUT1
            //按递归次数(目录深度)打印空格数量
            for(int i = 0; i < recur_cnt; i++){
                printf("  ");
            }
            //目录输出目录名
            printf("-%s\n", entry->d_name);
#endif
            //递归调用
            if(scanFile(curDir) < 0){
                printf("递归调用错误\n");
            }
        }else{
            //非目录类型

            //创建一个新的文件节点，将其窜到szDir->child节点或者next节点上
            struct Filenode *curDir = (struct Filenode *) calloc(sizeof(struct Filenode),1);
            if(i == 2){
                szDir->child = curDir;
            }else if(i > 2){
                if(NULL != oldDir){
                    oldDir->next = curDir;
                }else{
                    perror("oldDir is NULL");
                    //T.B.D 释放已经分配的curDir
                    return -1;
                }
            }
            oldDir = curDir;
            
            //从一个当前目录名，变成绝对路径
            int len = strlen(szDir->name);
            strncpy(curDir->name, szDir->name, sizeof(curDir->name));
            if (szDir->name[len - 1] != '/') 
                strncat(curDir->name, "/", 2);
            strcat(curDir->name, entry->d_name);
            curDir->type = entry->d_type;

#ifdef OUTPUT1
            //按递归次数(目录深度)打印空格数量
            for(int i = 0; i < recur_cnt; i++){
                printf("  ");
            }
            //非目录输出文件名
            printf("-%s\n", entry->d_name);
#endif

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

    struct Filenode root={0,};
    strncpy(root.name,dir,sizeof(root.name));
    root.type |= DT_DIR;
    scanFile(&root);

    return 0;
}