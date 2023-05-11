#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define     DIRFIRST    2
#define     RESULT2     2
#define     RESULT1     1
#define     ARGC        4

struct Filenode
{
    unsigned char type;
    char fullname[256];
    char name[256];
    char result[24];
    char new_flag;
    struct Filenode *child;
    struct Filenode *next;
};

int compareTraverse(struct Filenode *szDir1, struct Filenode *szDir2, struct Filenode *rsltDir);

int recur_cnt = 0;
int scanFile(struct Filenode *szDir) 
{
    struct dirent **entry_list;
    int count;
    int i;
    int len;

    recur_cnt++;
    if (szDir == NULL) {
        perror("szDir is NULL") ;
        return -1;
    }
    count = scandir(szDir->fullname, &entry_list, 0, alphasort) ;
    if (count < 0) {
        fprintf(stderr, "scandir [%s] error,%s\n", szDir->fullname, strerror(errno) ) ;
        return EXIT_FAILURE;
    }
    struct Filenode *oldDir = NULL;
    for (i = 0; i < count; i++) {
        struct dirent *entry;
        entry = entry_list[i];
        if (entry->d_type & DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            struct Filenode *curDir = (struct Filenode *) calloc(sizeof(struct Filenode) , 1) ;
            if (i == DIRFIRST) {
                szDir->child = curDir;
            }
            else if (i > DIRFIRST) {
                if (NULL != oldDir) {
                    oldDir->next = curDir;
                } else {
                    perror("oldDir is NULL") ;
                    return -1;
                }
            }
            oldDir = curDir;
            len = strlen(szDir->fullname) ;
            strncpy(curDir->fullname, szDir->fullname, sizeof(curDir->fullname) ) ;
            if (szDir->fullname[len - 1] != '/') {
                strncat(curDir->fullname, "/", 2) ;
            }
            strcat(curDir->fullname, entry->d_name) ;
            strncpy(curDir->name, entry->d_name, sizeof(curDir->name) ) ;
            curDir->type |= DT_DIR;
            if (scanFile(curDir) < 0) {
                printf("递归调用错误\n") ;
            }
        } else {
            struct Filenode *curDir = (struct Filenode *) calloc(sizeof(struct Filenode) , 1) ;
            if (i == DIRFIRST) {
                szDir->child = curDir;
            }else if (i > DIRFIRST) {
                if (NULL != oldDir) {
                    oldDir->next = curDir;
                } else {
                    perror("oldDir is NULL") ;
                    return -1;
                }
            }
            oldDir = curDir;
            len = strlen(szDir->fullname) ;
            strncpy(curDir->fullname, szDir->fullname, sizeof(curDir->fullname) ) ;
            if (szDir->fullname[len - 1] != '/') {
                strncat(curDir->fullname, "/", 2) ;
            }
            strcat(curDir->fullname, entry->d_name) ;
            strncpy(curDir->name, entry->d_name, sizeof(curDir->name) ) ;
            curDir->type = entry->d_type;
        }
        free(entry) ;
    } /* end of for */
    free(entry_list) ;
    recur_cnt--;
    return 0;
}
int traver_cnt = 0;
void PreOderTraverse(struct Filenode *szDir, int flag, FILE *fd) 
{
    char buf[512];
    memset(buf,0,sizeof(buf));

    if (szDir == NULL) {
        return;
    }
    if (flag == 1) {
        traver_cnt++;
    }
    if (fd == NULL) {
        for (int i = 0; i < traver_cnt; i++) {
            printf("  ") ;
        }
        if (szDir->result == NULL) {
            printf("|-%s\n", szDir->name) ;
        } else {
            printf("|-%s\t%s\n", szDir->name, szDir->result) ;
        }
    } else {
        if (strcmp(szDir->result, "same") != 0) {
            snprintf(buf, sizeof(buf) , "%s,", szDir->fullname) ;
            fwrite(buf, 1, sizeof(buf) , fd) ;
        }
    }
    PreOderTraverse(szDir->child, 1, fd) ;
    PreOderTraverse(szDir->next, 0, fd) ;
    if (flag == 1) {
        traver_cnt--;
    }
}

void compPreOderTraverse(struct Filenode *szDir, int flag, int result) 
{
    if (szDir == NULL) {
        return;
    }

    if (result == 0) {
        strcpy(szDir->result, "nocor") ;
    } else if (result == 1) {
        strcpy(szDir->result, "miss") ;
    } else if (result == 2) {
        strcpy(szDir->result, "ladiff") ;
    }
    compPreOderTraverse(szDir->child, 1, result) ;
    compPreOderTraverse(szDir->next, 0, result) ;
}
int CompareFile(const char *file1, const char *file2) 
{
    FILE *f1, *f2;
    int size1, size2;
    unsigned char buffer1[1024], buffer2[1024];
    int isTheSame;

    f1 = fopen(file1, "rb+") ;
    f2 = fopen(file2, "rb+") ;
    isTheSame = -1;

    if (!f1 || !f2) {
        goto end;
    }
    while (1) {
        size1 = fread(buffer1, 1, sizeof(buffer1) , f1) ;
        size2 = fread(buffer2, 1, sizeof(buffer2) , f2) ;
        if (size1 != size2) {
            goto end;
        }
        if (size1 < 1) {
            break;
        }
        for (int i = 0; i < size1; i++) {
            if (buffer1[i] != buffer2[i]) {
                goto end;
            }
        }
    }
    isTheSame = 0;
end:
    if (f1) {
        fclose(f1) ;
    }

    if (f2) {
        fclose(f2) ;
    }
    return isTheSame;
}

int compareDeep(struct Filenode *compSource, struct Filenode *compTarget, struct Filenode *newDir)
{
    int result,ret;
    struct stat statbuf1, statbuf2;
    ssize_t len1, len2;
    char buf1[512], buf2[512];

    memset(buf1,0,sizeof(buf1));
    memset(buf2,0,sizeof(buf2));
    result = 0;len1 = 0; len2 = 0;
    if (compTarget->type == DT_REG) {
        if (CompareFile(compSource->fullname, compTarget->fullname) == 0) {
            if (stat(compSource->fullname, &statbuf1) == -1) {
                printf("Get stat on %s Error: %s\n",
                compSource->fullname, strerror(errno) ) ;
                return (-1) ;
            }
            if (stat(compTarget->fullname, &statbuf2) == -1) {
                printf("Get stat on %s Error: %s\n",
                compTarget->fullname, strerror(errno) ) ;
                return (-1) ;
            }
            if ((strcmp(ctime(&(statbuf1.st_mtime)),
                        ctime(&(statbuf2.st_mtime)))!=0) 
                    || (statbuf1.st_mode != statbuf2.st_mode) ) {
                result = RESULT2;
                strcpy(newDir->result, "lidiff") ;
            } else {
                strcpy(newDir->result, "same") ;
            }
        } else {
            result = RESULT1;
            strcpy(newDir->result, "ladiff") ;
        }
    } else if (compTarget->type == DT_DIR) {
        ret = compareTraverse(compTarget, compSource, newDir) ;
        if (ret == 0) {
            strcpy(newDir->result, "same") ;
        }
        else if (ret == 1) {
            result = RESULT1;
            strcpy(newDir->result, "ladiff") ;
        } else {
            result = RESULT2;
            strcpy(newDir->result, "lidiff") ;
        }
    } else if (compTarget->type == DT_LNK) {
        if (((len1 = readlink(compTarget->fullname, buf1,sizeof(buf1) - 1)) != -1) 
                && ((len2 = readlink(compSource->fullname, buf2,sizeof(buf2) - 1)) != -1)){
            if (len1 == len2 && (strcmp(buf1, buf2) == 0) ) {
                strcpy(newDir->result, "same") ;
            } else {
                strcpy(newDir->result, "ladiff") ;
            }
        } else {
            printf("Get readlink Error: %s\n", strerror(errno) ) ;
            return (-1) ;
        }
    } else {
        result = RESULT1;
        strcpy(newDir->result, "tydiff") ;
    }
    return result;
}

int compareTraverse(struct Filenode *szDir1, struct Filenode *szDir2, struct Filenode *rsltDir) 
{
    int result,crCmpRst,lastCmpRst;
    struct Filenode *compSource,*compTarget,*newDir,*node;

    result = 0;
    compSource = szDir2->child;
    compTarget = szDir1->child;

    if (szDir1 == NULL || szDir2 == NULL) {
        return 0;
    }
    if (compSource == NULL) {
        if (compTarget != NULL) {
            newDir = (struct Filenode *) calloc(sizeof(struct Filenode) , 1) ;
            memcpy(newDir, compTarget, sizeof(struct Filenode) ) ;
            compPreOderTraverse(newDir, 0, 0) ;
            rsltDir->child = newDir;
            return -1;
        } else {
            return 0;
        }
    }    
    node = NULL;
    while (compSource != NULL || compTarget != NULL) {
        newDir = (struct Filenode *)calloc(sizeof(struct Filenode),1);
        lastCmpRst = 1;
        if ( (compTarget == NULL) || (compSource == NULL)) {
            if (compTarget == NULL) {
                memcpy(newDir, compSource, sizeof(struct Filenode) ) ;
                compPreOderTraverse(newDir, 0, 1) ;
                compSource = compSource->next;
            }else{
                memcpy(newDir, compTarget, sizeof(struct Filenode) ) ;
                compPreOderTraverse(newDir, 0, 0) ;
                compTarget = compTarget->next;
            }
            result = 1;
        } else {
            crCmpRst = strcmp(compSource->name, compTarget->name) ;
            if (crCmpRst > 0 && lastCmpRst > 0) {
                memcpy(newDir, compTarget, sizeof(struct Filenode) ) ;
                compPreOderTraverse(newDir, 0, 0) ;
                lastCmpRst = crCmpRst;
                result = 1;
                compTarget = compTarget->next;
            }
            else if (crCmpRst < 0 && lastCmpRst > 0) {
                memcpy(newDir, compSource, sizeof(struct Filenode) ) ;
                compPreOderTraverse(newDir, 0, 1) ;
                result = 1;
                compSource = compSource->next;
            } else if (crCmpRst == 0) {
                memcpy(newDir, compTarget, sizeof(struct Filenode) ) ;
                if (compTarget->type != compSource->type) {
                    compPreOderTraverse(newDir, 0, 2) ;
                    result = 1;
                }
                else if (compTarget->type == compSource->type) {
                    result = compareDeep(compSource,compTarget,newDir);
                } /* end of if */
                compSource = compSource->next;
                compTarget = compTarget->next;
            }/* end of if */
        } /* end of if */
        if (node == NULL) {
            rsltDir->child = newDir;
        } else {
            node->next = newDir;
        }
        node = newDir;
    } /* end of if */

    return result;
}

int main(int argc, char **argv) 
{
    char dir1[256];
    char dir2[256];
    struct Filenode root;
    struct Filenode root2;
    struct Filenode rsltDir;
    FILE *fd;

    memset(dir1,0,sizeof(dir1) ) ;
    memset(dir2,0,sizeof(dir2) ) ;
    memset(&root,0,sizeof(root) ) ;
    memset(&root2,0,sizeof(root2) ) ;
    memset(&rsltDir,0,sizeof(rsltDir) ) ;

    if (argc != 3 && argc != 4) {
        printf("Usage: %s folder1 folder2 [x] \n", argv[0]) ;
        printf("argc %d ,argv :%s %s %s %s \n", argc, argv[0], argv[1], argv[2], argv[3]) ;
        return 0;
    }

    strcpy(dir1, argv[1]) ;
    strcpy(dir2, argv[2]) ;

    strncpy(root.fullname, dir1, sizeof(root.fullname) ) ;
    strcpy(root.name, strrchr(dir1, '/') ) ;
    root.type |= DT_DIR;
    scanFile(&root) ;

    strncpy(root2.fullname, dir2, sizeof(root2.fullname) ) ;
    strcpy(root2.name, strrchr(dir2, '/') ) ;
    root2.type |= DT_DIR;
    scanFile(&root2) ;

    strcpy(rsltDir.name, "compileResult") ;
    compareTraverse(&root, &root2, &rsltDir) ;
    if (argc == ARGC && strcmp(argv[3], "x") == 0) {
        fd = fopen("/tmp/result.csv", "w+") ;
        PreOderTraverse(&rsltDir, 0, fd) ;
        fclose(fd) ;
    } else {
        PreOderTraverse(&rsltDir, 0, NULL) ;
    }
    return 0;
}
