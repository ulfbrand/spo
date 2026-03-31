#ifndef ERR_LIST_H
#define ERR_LIST_H

#define BUFFER_SIZE 1024

#define ERROR "\033[1m\033[41mERROR\033[0m"
#define WARN "\033[1m\033[43mWARNING\033[0m"


typedef struct err_list {
    char* err;
    struct err_list* next;
} err_list_t;

void push_err(err_list_t** list, char* format, ...);
void print_errors(err_list_t* l);

#endif