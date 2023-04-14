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
    char fullname[256];     //文件、文件夹的全路径名称 比如： rootdir/dir1/dia2/folder
    char name[256];     //文件、文件夹的名称 比如： folder
    char result[24];    //比较的结果
    char new_flag;      //是否是第二个比较目录的
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
    count = scandir(szDir->fullname, &entry_list, 0, alphasort);
    if (count < 0) {
        fprintf(stderr,"scandir [%s] error,%s\n",szDir->fullname,strerror(errno));
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
            int len = strlen(szDir->fullname);
            strncpy(curDir->fullname, szDir->fullname, sizeof(curDir->fullname));
            if (szDir->fullname[len - 1] != '/') 
                strncat(curDir->fullname, "/", 2);
            strcat(curDir->fullname, entry->d_name);
            strncpy(curDir->name, entry->d_name, sizeof(curDir->name));
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
            int len = strlen(szDir->fullname);
            strncpy(curDir->fullname, szDir->fullname, sizeof(curDir->fullname));
            if (szDir->fullname[len - 1] != '/') 
                strncat(curDir->fullname, "/", 2);
            strcat(curDir->fullname, entry->d_name);
            strncpy(curDir->name, entry->d_name, sizeof(curDir->name));
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

//前序遍历输出目录结构
int traver_cnt = 0;  //递归次数，就是目录深度
void PreOderTraverse(struct Filenode *szDir,int flag)
{
    if(szDir == NULL){
        return;
    }

    if(flag == 1)
        traver_cnt++;
    //显示结点数据，可以更改为其他对结点操作
    //按递归次数(目录深度)打印空格数量
    for(int i = 0; i < traver_cnt; i++){
        printf("  ");
    }
    //非目录输出文件名
    printf("-%s\n", szDir->name);
    PreOderTraverse(szDir->child,1);  //先遍历左子树（遍历子目录）
    PreOderTraverse(szDir->next,0);   //后遍历右子树 （遍历同级目录）

    if(flag == 1)
        traver_cnt--;
 } 

void compareFolderNode(struct Filenode *szDir1,struct Filenode *szDir2)
{

}


//比较两个目录szDir1，szDir2，以szDir1作为base, 比较结果保持到rslDir
 void compareTraverse(struct Filenode *szDir1,struct Filenode *szDir2,struct Filenode *rsltDir)
 {
    if(szDir1 == NULL || szDir2 == NULL){
        return;
    }

    //取第二个目录的第一个文件作为比较源，与第一个目录下的所有文件相比较
    struct Filenode * compSource  = szDir2->child;
    struct Filenode * compTarget  = szDir1->child;
    if(compSource == NULL){
        //如果第二个目录的内容为空
    
        if(compTarget != NULL){
            //如果第一个目录的内容不为空
            //创建一个新的文件节点，内容和第一个目录的child节点相同
            struct Filenode *newDir = (struct Filenode *) calloc(sizeof(struct Filenode),1);
            memcpy(newDir,compTarget,sizeof(struct Filenode));
            //串到rsltDir中
            rsltDir->child = newDir;
        }else{
            //两个目录都为空，啥都不做

        }
    }else{
        //如果第二个目录的内容非空

        if(compTarget == NULL){
            //如果第一个目录为空 
            //创建一个新的文件节点，内容和第二个目录的child节点相同，但是要标记一下是新增节点
            struct Filenode *newDir = (struct Filenode *) calloc(sizeof(struct Filenode),1);
            memcpy(newDir,compSource,sizeof(struct Filenode));
            //但是要标记一下是新增节点
            newDir->new_flag = 1;

            //串到rsltDir中
            rsltDir->child = newDir;
        }else{

            //如果第二个目录的内容非空，第一个目录也非空 开始循环比较
            while(compSource != NULL){
            {
                //如果第一个目录非空，开始循环比较
                while(compTarget != NULL){
                    //获取比较结果
                    int crCmpRst = strcmp(compTarget->name,compSource->name);
                    if( crCmpRst < 0 ){
                        //由于第一个目录中的文件名是从小到大排序的
                        continue;
                    }else if(crCmpRst > 0 ){

                    }else if( crCmpRst == 0 && (compTarget->type == compSource->type))
                    {
                        //文件、文件夹名称相同，且类型相同时，是可以深入比较的，否则就不同
                        //创建一个新的文件节点，将其窜到szDir->child节点或者next节点上
                        struct Filenode *newDir = (struct Filenode *) calloc(sizeof(struct Filenode),1);
                        memcpy(newDir,compTarget,sizeof(struct Filenode));
                        rsltDir->child = rsltDir;
                        compareTraverse(compTarget,compSource,rsltDir);

                        break;
                    }
                    compTarget = compTarget->next;
                }
                
                //取第二个目录的下一个文件
                compSource = compSource->next;
            }
        }



            

        }
    }



 }

int main(int argc,char** argv)
{
    char * dir = "/home/chenxy/test";
    //输出根目录
    printf("%s\n",dir);

    //创建根目录节点
    struct Filenode root={0,};
    strncpy(root.fullname,dir,sizeof(root.fullname));
    strcpy(root.name,"test");
    root.type |= DT_DIR;
    //通过扫描目录，拓展根目录节点
    scanFile(&root);

    //前序遍历目录节点
    PreOderTraverse(&root,0);

    struct Filenode rsltDir={0,};
    compareTraverse(&root,&root,&rsltDir);

    return 0;
}