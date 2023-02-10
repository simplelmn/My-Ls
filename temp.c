#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#ifndef STRUCT_LISTNODE
#define STRUCT_LISTNODE
typedef struct s_listnode
{
    char *data;
    struct stat filestat;
    struct s_listnode *next;
} listnode;
#endif

int my_size(const char *string){
    int i = 0;
    while(string[i]) i++;
    return i;
}

int my_strcmp(const char *str1, const char *str2){
    for(int i = 0; str1[i] != '\0' || str2[i] != '\0'; i++){
        if(str1[i] != str2[i])
            return str2[i] - str1[i];
        if(str1[i] == '\0')
            return 0;
        if(str2[i] == '\0')
            return -1;
    }
    return 0;
}

int my_compare(listnode *node_1, listnode *node_2)
{
    return my_strcmp(node_1->data, node_2->data);
}

listnode *create_node(listnode *list, listnode *node)
{
    listnode *previous, *next;
    if (!list)
    {
        list = node;
    }
    else
    {
        previous = NULL;
        next = list;
        while (next && my_compare(node, next) < 0)
        {
            previous = next;
            next = next->next;
        }
        if (!next)
        {
            previous->next = node;
        }
        else
        {
            if (previous)
            {
                node->next = previous->next;
                previous->next = node;
            }
            else
            {
                node->next = list;
                list = node;
            }
        }
    }
    return list;
}

int compare_mtim(listnode *list, listnode *node){
    if(list->filestat.st_mtim.tv_sec == node->filestat.st_mtim.tv_sec){
        if(list->filestat.st_mtim.tv_nsec < node->filestat.st_mtim.tv_nsec)
            return -1;
    }else{
         if(list->filestat.st_mtim.tv_sec < node->filestat.st_mtim.tv_sec)
            return -1;
    }
    return 0;
}

listnode *create_node_by_time(listnode *list, listnode *node)
{
    listnode *previous = NULL, *next = NULL;
    if (!list)
    {
        list = node;
    }
    else
    {
        previous = NULL;
        next = list;
        while (next && compare_mtim(node, next) < 0)
        {
            
            previous = next;
            next = next->next;
        }
        
        if (!next)
        {
            previous->next = node;
        }
        else
        {
            if (previous)
            {
                node->next = previous->next;
                previous->next = node;
            }
            else
            {
                node->next = list;
                list = node;
            }
        }
    }
    return list;
}


void my_putchar(char c)
{
    write(1, &c, 1);
}

void my_put_str(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        my_putchar(str[i]);
    }
}

void print_list(listnode *node)
{
    listnode *temp_node = node;
    while (temp_node != NULL)
    {
        my_put_str(temp_node->data);
        temp_node = temp_node->next;
        my_putchar('\n');
    }
}

int is_dir(char *dir_name)
{
    int status = 0;
    DIR *folder;
    folder = opendir(dir_name);
    
    if(folder == NULL){
        printf("my_ls: cannot access '%s': No such file or directory\n", dir_name);
    }else{
        status = 1;
    }
    closedir(folder);
    return status;
}

void sort_my_arguments(char **arguments, int ind)
{
    for(int i = 0; i < ind - 1; i++){
        for(int j = i + 1; j < ind; j++){
            if(my_strcmp(arguments[i], arguments[j]) < 0){
               char *temp = arguments[i];
               arguments[i] = arguments[j];
               arguments[j] = temp;
            }
        }
    }
}

void _directories__(char *dir_name, int *a_flag, int *t_flag)
{
    DIR *folder;
    struct dirent *entry; 
    struct stat filestat;
    listnode *head_d, *directories;
    head_d = NULL;

    folder = opendir(dir_name);

    if(*a_flag == 0 && *t_flag == 0)
    {
         while ((entry = readdir(folder)))
         {
            stat(entry->d_name, &filestat);
            if (entry->d_name[0] != '.')
            {
                directories = (listnode *)malloc(sizeof(listnode));
                directories->data = entry->d_name;
                directories->next = NULL;
                head_d = create_node(head_d, directories);
            }
        }
        print_list(head_d);
        
    }else
    if(*a_flag == 1 && *t_flag == 0){
         while ((entry = readdir(folder)))
        {
            stat(entry->d_name, &filestat);
            directories = (listnode *)malloc(sizeof(listnode));
            directories->data = entry->d_name;
            directories->next = NULL;
            head_d = create_node(head_d, directories);
        }
        print_list(head_d);
    }else
    if(*a_flag == 0 && *t_flag == 1){
         while ((entry = readdir(folder)))
        {
            stat(entry->d_name, &filestat);
            if (entry->d_name[0] != '.')
            {
                directories = (listnode *)malloc(sizeof(listnode));
                directories->data = entry->d_name;
                directories->filestat = filestat;
                directories->next = NULL;
                head_d = create_node_by_time(head_d, directories);
            }
        }
        print_list(head_d);
    }else
    if(*a_flag == 1 && *t_flag == 1){
        while ((entry = readdir(folder)))
        {
            stat(entry->d_name, &filestat);
            directories = (listnode *)malloc(sizeof(listnode));
            directories->data = entry->d_name;
            directories->filestat = filestat;
            directories->next = NULL;
            head_d = create_node_by_time(head_d, directories);
        }
        print_list(head_d); 
    }
    free(directories);    
}

int my_ls(int argc, char *argv[])
{
    int a_flag = 0, t_flag = 0;
    char **arguments = (char**)malloc(100 * sizeof(char*));
    int ind = 0;
    for (int i = 1; i < argc; i++){
        if(my_strcmp(argv[i], "-t") == 0){
            t_flag = 1;
        }else
        if(my_strcmp(argv[i], "-a") == 0){
            a_flag = 1;
        }else
        if(my_strcmp(argv[i], "-at") == 0 || my_strcmp(argv[i], "-ta") == 0){
            a_flag = 1;
            t_flag = 1;
        }else{
            arguments[ind] = (char*)malloc(20 * sizeof(char));
            arguments[ind] = argv[i];
            ind++;
        }   
    }

    
    if(ind == 0){
        arguments[0] = ".";
        _directories__( arguments[0],&a_flag, &t_flag);
    }
    else if(ind == 1){
        if(is_dir(arguments[0])){
            _directories__(arguments[0], &a_flag, &t_flag);
        }
    }else{
        sort_my_arguments(arguments, ind);

        for(int i = 0; i < ind; i++){
            if(is_dir(arguments[i]) == 1){
                printf("%s:\n", arguments[i]);
                fflush(stdout);
            }
            if(is_dir(arguments[i])){
                _directories__(arguments[i], &a_flag, &t_flag);
            }
            if(i != ind - 1){
                printf("\n");
                fflush(stdout);
            }
        }
    }
    return 0;   
}

int main(int argc, char *argv[])
{
    my_ls(argc, argv);
    return 0;
}