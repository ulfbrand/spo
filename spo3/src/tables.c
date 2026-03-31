#include "tables.h"
#include <string.h>
#include "type.h"

int cmp_args(func_sig_t* f, func_sig_t* s) {
    if(f->args_cnt != s->args_cnt) return -1;
    for (uint32_t i = 0; i < f->args_cnt; ++i) {
        if (strcmp(f->arg_names[i], s->arg_names[i]) != 0) return -1;
        if (cmp_type(f->args_types[i], s->args_types[i]) != 0) return -1;
    }
    return 0;
}

find_status_e find_signature(func_sig_t* sig, func_sig_t* list) {
    while (list) {
        if (strcmp(sig->name, list->name) != 0) {
            list = list->next;
        } else {
            if (cmp_args(sig, list) != 0 || cmp_type(sig->return_type, list->return_type) != 0) {
                return REDECLARATION;
            }
            if (sig->defined && list->defined) {
                return REDEFINITION;
            }
            return FOUND;
        }
    }
    return NOT_FOUND;
}

func_sig_t* find_signature_by_name(char* name, func_sig_t* list) {
    while (list) {
        if (strcmp(name, list->name) == 0) {
            return list;
        }
        list = list->next;
    }
    return list;
}

const_table_t* push_constant(struct type* type, char* val, const_table_t* rest) {
    if (find_const(val, rest)) return rest;
    const_table_t* new = malloc(sizeof(const_table_t));
    new->type = type;
    new->literal = strdup(val);
    new->next = rest;
    new->id = rest ? rest->id + 1 : 0;
    return new;
}

const_table_t* find_const(char* val, const_table_t* consts) {
    while(consts) {
        if (strcmp(consts->literal, val) == 0) return consts;
        consts = consts->next;
    }
    return NULL;
}

symb_table_t* get_var(char* name, symb_table_t* symb_table) {
    while (symb_table) {
        if (strcmp(symb_table->name, name) == 0) {
            return symb_table;
        }
        symb_table = symb_table->next;
    }
    return NULL;
}

symb_table_t* push_var(char* name, symb_table_t* symb_table) {
    symb_table_t* new = malloc(sizeof(symb_table_t));
    new->name = strdup(name);
    new->next = symb_table;
    new->addr = symb_table ? symb_table->addr + 2 : 0;
    return new;
}