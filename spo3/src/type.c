#include "type.h"

type_t* create_non_array_type(char* label) {
    type_t* curr_type = malloc(sizeof(type_t));
    if(strcmp(label, "BOOL") == 0) {
        curr_type->kind = BUILTIN;
        curr_type->builtin = MY_BOOL;
    } else if (strcmp(label, "BYTE") == 0) {
        curr_type->kind = BUILTIN;
        curr_type->builtin = MY_BYTE;
    } else if (strcmp(label, "INT") == 0) {
        curr_type->kind = BUILTIN;
        curr_type->builtin = MY_INT;
    } else if (strcmp(label, "UINT") == 0) {
        curr_type->kind = BUILTIN;
        curr_type->builtin = MY_UINT;
    } else if (strcmp(label, "LONG") == 0) {
        curr_type->kind = BUILTIN;
        curr_type->builtin = MY_LONG;
    } else if (strcmp(label, "ULONG") == 0) {
        curr_type->kind = BUILTIN;
        curr_type->builtin = MY_ULONG;
    } else if (strcmp(label, "CHAR") == 0) {
        curr_type->kind = BUILTIN;
        curr_type->builtin = MY_CHAR;
    } else if (strcmp(label, "STRING") == 0) {
        curr_type->kind = BUILTIN;
        curr_type->builtin = MY_STRING;
    } else if (strcmp(label, "VOID") == 0) {
        curr_type->kind = BUILTIN;
        curr_type->builtin = MY_VOID;
    } else {
        curr_type->kind = CUSTOM;
        curr_type->custom = strdup(label);
    }
    return curr_type;
}

type_t* parse_type(ast_t* ast, struct file_defs* defs) {
    if (ast->children_cnt > 2) {
        push_err(&defs->err, "%s: Jagged arrays are not supported (line %d, pos %d)", WARN, ast->children[0]->line, ast->children[0]->pos);
    }
    type_t* curr_type;
    if(ast->children_cnt > 1) {
        curr_type = malloc(sizeof(type_t));
        curr_type->kind = ARRAY;
        curr_type->array = malloc(sizeof(arr_type_t));
        curr_type->array->type = create_non_array_type(ast->children[0]->label);
        curr_type->array->dim = ast->children[1]->children_cnt + 1;
        return curr_type;
    }

    curr_type = create_non_array_type(ast->children[0]->label);
    return curr_type;

}

char* builtin_str[] = {
    [MY_BOOL] = "bool",
    [MY_BYTE] = "byte",
    [MY_INT] = "int",
    [MY_UINT] = "uint",
    [MY_LONG] = "long",
    [MY_ULONG] = "ulong",
    [MY_CHAR] = "char",
    [MY_STRING] = "string",
    [MY_VOID] = "void"
};

char* type_to_string(type_t* type) {
    switch (type->kind) {
        case BUILTIN:
            return builtin_str[type->builtin];
        case CUSTOM:
            return type->custom;
        case ARRAY: {
            char *buffer = malloc(20);
            sprintf(buffer, "%s[%d]", type_to_string(type->array->type), type->array->dim);
            return buffer;
        } 
    }
}

int cmp_type(type_t* f, type_t* s) {
    if (f->kind != s->kind) return -1;
    switch (f->kind) {
        case BUILTIN:
            if (f->builtin != s->builtin) return -1;
            return 0;
        case CUSTOM:
            return strcmp(f->custom, s->custom);
        case ARRAY: {
            if (f->array->dim != s->array->dim) return -1;
            return cmp_type(f->array->type, s->array->type);
        }
    }
    return 0;
}

type_t* detect_type(char* str) {
    type_t* type = malloc(sizeof(type_t));
    type->kind = BUILTIN;
    if (!str || strlen(str) == 0) {
        type->builtin = MY_VOID;
    } else if (str[0] == '\'') {
        type->builtin = MY_CHAR;
    } else if (str[0] == '\"') {
        type->builtin = MY_STRING;
    } else if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0) {
        type->builtin = MY_BOOL;
    } else {
        type->builtin = MY_INT;
    }
    return type;
}