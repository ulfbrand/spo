#include "err_list.h"
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

err_list_t* create_list(char* err) {
    err_list_t* l = malloc(sizeof(err_list_t));
    l->err = strdup(err);
    l->next = NULL;
    return l;
}

void push_err(err_list_t** list, char* format, ...) {
    char buf[BUFFER_SIZE];
 	va_list arg;
 	va_start(arg, format);

 	vsprintf(buf, format, arg);
    err_list_t* new = create_list(buf);

    new->next = *list;
    (*list) = new;

    va_end(arg);
}



void rev_print(err_list_t* l) {
    if(l) {
        rev_print(l->next);
        printf("%s\n", l->err);
    }
}
void print_errors(err_list_t* l) {
    printf("\n");
    rev_print(l);
    printf("\n");
}
