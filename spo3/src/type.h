#ifndef TYPE_H
#define TYPE_H

#include <stdint.h>
#include "cfg.h"

typedef enum {
    MY_BOOL = 0,
    MY_BYTE = 1,
    MY_INT = 2,
    MY_UINT = 3,
    MY_LONG = 4,
    MY_ULONG = 5,
    MY_CHAR = 6,
    MY_STRING = 7,
    MY_VOID = 8
} builtin_e;

typedef enum {
    BUILTIN,
    CUSTOM,
    ARRAY,
} type_e;

typedef struct {
    struct type* type;
    uint32_t dim;
} arr_type_t;

typedef struct type {
    type_e kind;
    union {
        builtin_e builtin;
        char* custom;
        arr_type_t* array;
    };
} type_t;

type_t* create_non_array_type(char* label);
type_t* detect_type(char* str);
struct file_defs;
type_t* parse_type(ast_t* ast, struct file_defs* defs);

char* type_to_string(type_t* type);

int cmp_type(type_t* f, type_t* s);
#endif