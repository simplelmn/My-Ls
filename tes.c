#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

typedef struct s_listnode
{
    char* f_name;
    struct stat filestat;
    struct s_listnode* next;
} listnode;

void print_list(listnode* list){
    while (list) {
        printf("%s", list->f_name);
        list = list->next;
        printf("\n");
    }
}

int  my_strcmp(char *s1, char *s2){

    for(int i = 0 ; s1[i] || s2[i] ; i++){
        if(s1[i] != s2[i]){
            return s2[i] - s1[i];
        }
        if(!s1[i]) return 0;
        if(!s2[i]) return -1;
    }
    return 0;
}

void sort_dirs(char **dirs, int index){
    for(int i = 0; i < index - 1; i++){
        for(int j = i + 1; j < index; j++){
            if(my_strcmp(dirs[i], dirs[j]) < 0){
                char *t = dirs[i];
                dirs[i] = dirs[j];
                dirs[j] = t;
            }
        }
    }
}

void add_last(listnode **head, char *value){
    struct stat filestat;
    listnode *new_node = (listnode*)malloc(sizeof(listnode));
    stat(value, &filestat);
    new_node->filestat = filestat;
    new_node->f_name = value;
    new_node->next = NULL;
    if(*head == NULL)
        *head = new_node;
    else
    {
        listnode *last_node = *head;
        while (last_node->next != NULL) {
            last_node = last_node->next;
        }
        last_node->next = new_node;
    }
}


listnode* sorting_alph(listnode* head1, listnode* head2){
    listnode *pv, *nt;
    if(!head1) head1 = head2;
    else{
        pv = NULL;
        nt = head1;
        while(nt && my_strcmp(head2->f_name, nt->f_name) < 0){
            pv = nt;
            nt = nt->next;
        }
        if(!nt){
            pv->next = head2;
        }else{
            if(pv){
                head2->next = pv->next;
                pv->next = head2;
            }else{
                head2->next = head1;
                head1 = head2;
            }
        }
    }
    return head1;
}

int my_mtim(listnode *first, listnode *second){
    if(first->filestat.st_mtim.tv_sec == second->filestat.st_mtim.tv_sec){
        if(first->filestat.st_mtim.tv_nsec < second->filestat.st_mtim.tv_nsec) 
            return -1;
    }else{
        if (first->filestat.st_mtim.tv_sec < second->filestat.st_mtim.tv_sec)
            return -1;
    }
    return 0;
}

listnode* sort_by_time(listnode* head1, listnode* head2){
     listnode *pv, *nt;
    if(!head1) head1 = head2;
    else{
        pv = NULL;
        nt = head1;
        while(nt && my_mtim(head2, nt) < 0){
            pv = nt;
            nt = nt->next;
        }
        if(!nt){
            pv->next = head2;
        }else{
            if(pv){
                head2->next = pv->next;
                pv->next = head2;
            }else{
                head2->next = head1;
                head1 = head2;
            }
        }
    }
    return head1;
}

void get_content(char* dir, int a, int t){
    listnode* content = NULL, *dirs;
    DIR *fold;
    struct stat fs;
    struct dirent *entry;
    
    fold = opendir(dir);
    
    if(a==0 && t == 0){
        while((entry = readdir(fold))){
            if(entry->d_name[0] != '.'){
                dirs = (listnode*)malloc(sizeof(listnode));
                dirs->f_name = entry->d_name;
                dirs->next = NULL;
                content = sorting_alph(content, dirs);
            }
        }
        print_list(content);
    }else if(a == 1 && t == 0){
        while((entry = readdir(fold))){
            dirs = (listnode*)malloc(sizeof(listnode));
            dirs->f_name = entry->d_name;
            dirs->next = NULL;
            content = sorting_alph(content, dirs);
        }
        print_list(content);
    }else if(a == 0 && t == 1){
        while((entry = readdir(fold))){
            if(entry->d_name[0] != '.'){
                stat(entry->d_name, &fs);
                dirs = (listnode*)malloc(sizeof(listnode));
                dirs->f_name = entry->d_name;
                dirs->filestat = fs;
                dirs->next = NULL;
                content = sort_by_time(content, dirs);
            }
        }
        print_list(content);
    }else if(a == 1 && t == 1){
        while((entry = readdir(fold))){
            stat(entry->d_name, &fs);
            dirs = (listnode*)malloc(sizeof(listnode));
            dirs->f_name = entry->d_name;
            dirs->filestat = fs;
            dirs->next = NULL;
            content = sort_by_time(content, dirs);
        }
        print_list(content);
    }
    closedir(fold);
}

int find_dir(char *name){
    int i = 0;
    DIR *fold;
    fold = opendir(name);
    if(fold == NULL)
        printf("my_ls: cannot access '%s': No such file or directory\n", name);
    else i = 1;
    closedir(fold);
    return i;
}

int main(int ac, char** av){
    int t = 0, a = 0;
    char **array = (char**)malloc(300 * sizeof(char*));
    int j = 0;
    for(int i = 1 ; i < ac; i++){
        if(strcmp("-a", av[i]) == 0) 
            a = 1;
        else if(strcmp("-t", av[i]) == 0) 
            t = 1;
        else if(strcmp("-at", av[i]) == 0 || strcmp("-ta", av[i]) == 0){
            a = 1;
            t = 1;
        }else{
            array[j] = (char*)malloc(50 * sizeof(char));
            array[j] = av[i];
            j++;
        }
    }
    if(j == 0){
        array[0] = ".";
        get_content(array[0], a, t);
    }else if(j == 1){
        get_content(array[0], a, t);
    }else{
         sort_dirs(array, j);
        for(int i = 0; i < j; i++){
            if(find_dir(array[i]) == 1){
                printf("%s:\n", array[i]);
                fflush(stdout);
            }
            if(find_dir(array[i])){
                get_content(array[i], a, t);
            }
            if(i != j - 1){
                printf("\n");
                fflush(stdout);
            }
        }
    }
    return 0;
}
