#ifndef TABLES_H
#define TABLES_H

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t addr_t;
struct type;
struct cfg_block;

typedef struct symb_table {
    struct type* type;
    char* name;
    addr_t addr;

    struct symb_table* next;
} symb_table_t;

typedef struct const_table {
    struct type* type;
    char* literal;
    uint32_t id;

    struct const_table* next;
} const_table_t;

typedef struct func_sig{
    char* name;
    struct type* return_type;
    uint32_t args_cnt;
    struct type ** args_types;
    char** arg_names;
    bool defined;
    struct cfg_block* cfg;
    bool translated;
    symb_table_t* start_scope;

    struct func_sig* next;
} func_sig_t;

typedef enum {
    NOT_FOUND,
    REDECLARATION,
    REDEFINITION,
    FOUND
} find_status_e;

find_status_e find_signature(func_sig_t* sig, func_sig_t* list);
func_sig_t* find_signature_by_name(char* name, func_sig_t* list);

const_table_t* push_constant(struct type* type, char* val, const_table_t* rest);

const_table_t* find_const(char* val, const_table_t* consts);

symb_table_t* get_var(char* name, symb_table_t* symb_table);
symb_table_t* push_var(char* name, symb_table_t* symb_table);


#endif