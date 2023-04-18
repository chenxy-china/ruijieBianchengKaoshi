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
/*
    参数1 struct Filenode *szDir ，目录结构指针
    参数2 int flag，表示是子目录还是同级目录，1：子目录，需要目录深度+-操作，0：同级目录，不做目录深度+-操作
*/
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
    if(szDir->result == NULL){
        printf("-%s\n", szDir->name);
    }else{
        printf("-%s\t%s\n", szDir->name,szDir->result);
    }
    

    PreOderTraverse(szDir->child,1);  //先遍历左子树（遍历子目录）
    PreOderTraverse(szDir->next,0);   //后遍历右子树 （遍历同级目录）

    if(flag == 1)
        traver_cnt--;
 } 

//前序遍历输出目录结构
int compTraver_cnt = 0;  //递归次数，就是目录深度
/*
    参数1 struct Filenode *szDir ，目录结构指针
    参数2 int flag，表示是子目录还是同级目录，1：子目录，需要目录深度+-操作，0：同级目录，不做目录深度+-操作
    参数3 int result, 表示比较结果，如果是0：文件只有base存在 ,1:文件只有target存在，2，两个都存在，但是不同 ,
*/
void compPreOderTraverse(struct Filenode *szDir,int flag,int result)
{
    if(szDir == NULL){
        return;
    }

    if(flag == 1)
        compTraver_cnt++;

    //非目录输出文件名
    if(result == 0){
    }else if(result == 1){
        strcpy(szDir->result,"+");
    }else if(result == 2){
        strcpy(szDir->result,"diff");
    }
    compPreOderTraverse(szDir->child,1,result);  //先遍历左子树（遍历子目录）
    compPreOderTraverse(szDir->next,0,result);   //后遍历右子树 （遍历同级目录）

    if(flag == 1)
        compTraver_cnt--;
 } 

void compareFolderNode(struct Filenode *szDir1,struct Filenode *szDir2)
{

}

/// 比较两个文件内容是否相同
/// </summary>
/// <param name="file1">文件1的路径</param>
/// <param name="file2">文件2的路径</param>
/// <returns>是否相同，0相同，非0不相同</returns>
int CompareFile(const char * file1, const char* file2)
{
	FILE* f1, * f2;
	int size1, size2;
	unsigned char buffer1[1024], buffer2[1024];
	f1 = fopen(file1, "rb+");
	f2 = fopen(file2, "rb+");
	int isTheSame = -1;
	if (!f1)
		goto end;
	if (!f2)
		goto end;
	while (1)
	{
		size1 = fread(buffer1, 1, 1024, f1);
		size2 = fread(buffer2, 1, 1024, f2);
		if (size1 != size2)
		{
			goto end;
		}
		if (size1 < 1)
			break;
		for (int i = 0; i < size1; i++)
		{
			if (buffer1[i] != buffer2[i])
			{
				goto end;
			}
		}
	}
	isTheSame = 0;
end:
	if (f1)
		fclose(f1);
	if (f2)
		fclose(f2);
	return isTheSame;
}

//比较两个目录szDir1，szDir2，以szDir1作为base, 比较结果保持到rslDir
int compareTraverse(struct Filenode *szDir1,struct Filenode *szDir2,struct Filenode *rsltDir)
{
    if(szDir1 == NULL || szDir2 == NULL){
        return 0;
    }

    compTraver_cnt++;
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
            compPreOderTraverse(newDir,1,0);
            //串到rsltDir中
            rsltDir->child = newDir;
            return -1;
        }else{
            //两个目录都为空，啥都不做
            return 0;
        }
    }else{
        //如果第二个目录的内容非空
        
        if(compTarget == NULL){
            //如果第一个目录为空 
            //说明第二个目录中这些个文件是第一个目录里没有的

            //创建一个新的文件节点，内容和第一个目录的child节点相同
            struct Filenode *newDir = (struct Filenode *) calloc(sizeof(struct Filenode),1);
            memcpy(newDir,compSource,sizeof(struct Filenode));
            compPreOderTraverse(newDir,1,1);
            //串到rsltDir中
            rsltDir->child = newDir;
            return -1;
        }else{

            struct Filenode * node = NULL;
            int breakf = 0; 
            //如果第二个目录的内容非空，第一个目录也非空 开始循环比较
            while(compSource != NULL){
                int lastCmpRst = 1;
                //如果第一个目录非空，开始循环比较
                while(compTarget != NULL){                    
                    /*strcmp(const char *str1, const char *str2) 
                     str1 小于 str2 返回值小于 0
                      str1 大于 str2 返回值大于 0
                      str1 等于 str2 返回值等于 0
                    */
                   //获取第二个目录中文件，和第一个目录中文件 比较结果
                    int crCmpRst = strcmp(compSource->name,compTarget->name);
                    if( crCmpRst > 0 && lastCmpRst > 0){
                        //说明compTarget这个文件/文件夹 ，只有第一个目录有
                        
                        //创建一个新的文件节点，内容和第一个目录的child节点相同
                        struct Filenode *newDir = (struct Filenode *) calloc(sizeof(struct Filenode),1);
                        memcpy(newDir,compTarget,sizeof(struct Filenode));
                        compPreOderTraverse(newDir,0,0);

                        if(node == NULL){
                            //串到rsltDir中
                            rsltDir->child = newDir;
                            node = newDir;
                        }else{
                            //串到rsltDir中
                            node->next = newDir;
                            node = newDir;
                        }
                        
                        //由于第一个目录中的文件名是从小到大排序的,所以需要compSource比较下一个compTarget
                        lastCmpRst = crCmpRst;
                        //goto next_file_in_folder1
                    }else if(crCmpRst < 0 && lastCmpRst > 0){
                        //由于第一个目录中的文件名是从小到大排序的,说明compTarget后面的字符串全都大于compSource
                        //说明compSource这个文件/文件夹 ，只有第二个目录有

                        //创建一个新的文件节点，内容和第一个目录的child节点相同
                        struct Filenode *newDir = (struct Filenode *) calloc(sizeof(struct Filenode),1);
                        memcpy(newDir,compSource,sizeof(struct Filenode));
                        compPreOderTraverse(newDir,0,1);

                        if(node == NULL){
                            //串到rsltDir中
                            rsltDir->child = newDir;
                            node = newDir;
                        }else{
                            //串到rsltDir中
                            node->next = newDir;
                            node = newDir;
                        }

                        break;  //goto next_file_in_folder2
                    }else if( crCmpRst == 0 ){
                        //文件、文件夹名称相同
                        //创建一个新的文件节点，内容和第一个目录的child节点相同
                        struct Filenode *newDir = (struct Filenode *) calloc(sizeof(struct Filenode),1);
                        memcpy(newDir,compTarget,sizeof(struct Filenode));
                        
                        if(compTarget->type != compSource->type){
                            //类型不同时
                            compPreOderTraverse(newDir,0,2);

                        }else if(compTarget->type == compSource->type){
                            //文件、文件夹名称相同，且类型相同时，是可以深入比较的，否则就不同

                            if(compTarget->type == DT_REG ){
                                //如果是常规文件文件类型，比较文件，输出比较结果
                                if(CompareFile(compSource->fullname,compTarget->fullname) == 0){
                                    strcpy(newDir->result,"same");
                                }else{
                                    strcpy(newDir->result,"diff");
                                }
                            }else if(compTarget->type == DT_DIR){
                                //如果是目录类型，递归比较该两个目录
                                if(compareTraverse(compTarget,compSource,newDir) == 0){
                                    strcpy(newDir->result,"same");
                                }else{
                                    strcpy(newDir->result,"diff");
                                }
                            }else{
                                strcpy(newDir->result,"type can`t cmp");
                            }            
                        }
                        if(node == NULL){
                            //串到rsltDir中
                            rsltDir->child = newDir;
                            node = newDir;
                        }else{
                            //串到rsltDir中
                            node->next = newDir;
                            node = newDir;
                        }
                        breakf = 1;
                    }
                    
//next_file_in_folder1:
                    //取第一个目录的下一个文件
                    compTarget = compTarget->next;
                    
                    if(breakf == 1){
                        breakf = 0;
                        break;
                    }
                }
//next_file_in_folder2:
                //取第二个目录的下一个文件
                compSource = compSource->next;
            }
        }
    }
    compTraver_cnt--;
    return 0;
}

int main(int argc,char** argv)
{
    char * dir = "/home/chenxy/test/ks-1";

    //创建根目录节点
    struct Filenode root={0,};
    strncpy(root.fullname,dir,sizeof(root.fullname));
    strcpy(root.name,"ks-1");
    root.type |= DT_DIR;
    //通过扫描目录，拓展根目录节点
    scanFile(&root);

    //前序遍历目录节点
    PreOderTraverse(&root,0);
    printf("++++++++++++++++++++++\n");

    //创建比较目录2根目录节点
    struct Filenode root2={0,};
    strncpy(root2.fullname,"/home/chenxy/Videos",sizeof(root.fullname));
    strcpy(root2.name,"Videos");
    root2.type |= DT_DIR;
    //通过扫描目录，拓展根目录节点
    scanFile(&root2);
    //前序遍历目录节点
    PreOderTraverse(&root2,0);

    printf("++++++++++++++++++++++\n");



    struct Filenode rsltDir={0,};
    strcpy(rsltDir.name,"compileResult");
    compareTraverse(&root2,&root,&rsltDir);

    //输出比较结果
    PreOderTraverse(&rsltDir,0);
    return 0;
}