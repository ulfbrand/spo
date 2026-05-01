#include "codegen.h"

struct funcs_queue {
    char* name;
    struct funcs_queue* next;
};

struct funcs_queue* queue_push(char* name, struct funcs_queue* tail) {
    struct funcs_queue* head = malloc(sizeof(struct funcs_queue));
    head->name = strdup(name);
    head->next = tail;
    return head;
}

bool queue_find(char* name, struct funcs_queue* tail) {
    while (tail) {
        if (strcmp(name, tail->name) == 0) return true;
        tail = tail->next;
    }
    return false;
}

struct funcs_queue* queue_pop(struct funcs_queue* queue) {
    struct funcs_queue* tail = queue->next;
    free(queue->name);
    free(queue);
    return tail;
}

enum ot_type {
    OT_STORE = 0,
    OT_LOAD_DATA = 1,
    OT_LOAD_CONST = 2,
    OT_ADD = 3,
    OT_SUB = 4,
    OT_MUL = 5,
    OT_DIV = 6,
    OT_MOD = 7,
    OT_AND = 8,
    OT_OR = 9,
    OT_LT = 10,
    OT_LTE = 11,
    OT_GT = 12,
    OT_GTE = 13,
    OT_EQ = 14,
    OT_NEQ = 15,
    OT_L_SHIFT = 16,
    OT_R_SHIFT = 17,
    OT_CALL = 18,
    OT_ARRAY = 19,
    OT_ERR = 20,
};

char* ot_type_to_mnem[] = {
    [OT_STORE] = "st",
    [OT_LOAD_DATA] = "ld_data",
    [OT_LOAD_DATA] = "ld_const",
    [OT_ADD] = "ADD",
    [OT_SUB] = "SUB",
    [OT_MUL] = "MUL",
    [OT_DIV] = "DIV",
    [OT_MOD] = "MOD",
    [OT_AND] = "BIT_AND",
    [OT_OR] = "OR",
    [OT_LT] = "LT",
    [OT_LTE] = "LTE",
    [OT_GT] = "GT",
    [OT_GTE] = "GTE",
    [OT_EQ] = "EQ",
    [OT_NEQ] = "NEQ",
    [OT_L_SHIFT] = "left_shift",
    [OT_R_SHIFT] = "right_shift",
    [OT_CALL] = "call",
    [OT_ARRAY] = "ARRAY",
    [OT_ERR] = "ERROR"
};

enum ot_type get_ot_type(char* label) {
    if (strcmp(label, "store") == 0) return OT_STORE;
    if (strcmp(label, "load_data") == 0) return OT_LOAD_DATA;
    if (strcmp(label, "load_const") == 0) return OT_LOAD_CONST;
    if (strcmp(label, "call") == 0) return OT_CALL;
    if (strcmp(label, "ARRAY") == 0) return OT_ARRAY;
    if (strcmp(label, "+") == 0) return OT_ADD;
    if (strcmp(label, "-") == 0) return OT_SUB;
    if (strcmp(label, "*") == 0) return OT_MUL;
    if (strcmp(label, "/") == 0) return OT_DIV;
    if (strcmp(label, "%") == 0) return OT_MOD;
    if (strcmp(label, "&") == 0) return OT_AND;
    if (strcmp(label, "|") == 0) return OT_OR;
    if (strcmp(label, "<") == 0) return OT_LT;
    if (strcmp(label, "<=") == 0) return OT_LTE;
    if (strcmp(label, ">") == 0) return OT_GT;
    if (strcmp(label, ">=") == 0) return OT_GTE;
    if (strcmp(label, "==") == 0) return OT_EQ;
    if (strcmp(label, "!=") == 0) return OT_NEQ;
    if (strcmp(label, "<<") == 0) return OT_L_SHIFT;
    if (strcmp(label, ">>") == 0) return OT_R_SHIFT;
    return OT_ERR;
}

void print_scope(symb_table_t* scope, FILE* file) {
    while (scope) {
        fprintf(file, ";scope %d : %s\n", scope->addr, scope->name);
        scope = scope->next;
    }
}

void generate_expr(ot_t* ot, symb_table_t* scope, FILE* file, file_defs_t* defs, struct funcs_queue** queue) {
    while (ot) {
        enum ot_type type = get_ot_type(ot->label);
        if (type == OT_ERR) {
            printf("Unexpected operation tree node\n");
        } else if (type == OT_CALL) {
            func_sig_t* sig = find_signature_by_name(ot->childs[0]->name, defs->signatures);
            if (sig) {
                if (ot->child_cnt > 1) {
                    ot_t* arg = ot->childs[1];
                    generate_expr(arg, scope, file, defs, queue);
                    for (uint32_t i = 0; i < sig->args_cnt; ++i) {
                        symb_table_t* var = get_var(sig->arg_names[sig->args_cnt - 1 - i], sig->start_scope); 
                        int scope_addr = scope ? scope->addr + 2 : 0;
                        fprintf(file, "store %d\t\t;\tcall arg %s\n", var->addr + scope_addr + 2, sig->arg_names[sig->args_cnt - 1 - i]);
                    }
                }
            }
            int scope_addr = scope ? scope->addr + 2: 0;
            // fprintf(file, ";debug\t %d\n", scope_addr);
            // print_scope(scope, file);
            fprintf(file, "CALL %s %d\n", ot->childs[0]->name, scope_addr + 2);
            if (!queue_find(ot->childs[0]->name, *queue)) {
                *queue = queue_push(ot->childs[0]->name, *queue);
            }
        } else if (type == OT_LOAD_CONST) {
            const_table_t* constant = find_const(ot->childs[0]->name, defs->const_table);
            fprintf(file, "load_const const_%d\n", constant->id);
        } else if (type == OT_LOAD_DATA) {
            if (strcmp(ot->childs[0]->label, "ARRAY") == 0) {
                generate_expr(ot->childs[0]->childs[1], scope, file, defs, queue);
                symb_table_t* var = get_var(ot->childs[0]->childs[0]->name, scope);
                if (!var) {
                    return;
                }
                fprintf(file, "load_data_by_index %d\n", var->addr);
            } else {
                symb_table_t* var = get_var(ot->childs[0]->name, scope);
                if (!var) {
                    return;
                }
                fprintf(file, "load_data %d\t\t;\tvar %s\n", var->addr, ot->childs[0]->name);
            }
        } else if (type == OT_STORE) {
            if (strcmp(ot->childs[0]->label, "ARRAY") == 0) {
                generate_expr(ot->childs[0]->childs[1], scope, file, defs, queue);
                symb_table_t* var = get_var(ot->childs[0]->childs[0]->name, scope);
                if (!var) {
                    return;
                }
                generate_expr(ot->childs[1], scope, file, defs, queue);
                fprintf(file, "store_by_index %d\n", var->addr);
            } else {
                symb_table_t* var = get_var(ot->childs[0]->name, scope);
                if (!var) {
                    return;
                }
                generate_expr(ot->childs[1], scope, file, defs, queue);
                fprintf(file, "store %d\t\t;\tvar %s\n", var->addr, ot->childs[0]->name);
            }
        } else {
            generate_expr(ot->childs[0], scope, file, defs, queue);
            generate_expr(ot->childs[1], scope, file, defs, queue);
            switch (get_ot_type(ot->label)){
                case OT_ADD:
                case OT_SUB:
                case OT_MUL:
                case OT_DIV:
                case OT_MOD:
                case OT_AND:
                case OT_OR:
                case OT_LT:
                case OT_LTE:
                case OT_GT:
                case OT_GTE:
                case OT_EQ:
                case OT_NEQ:
                case OT_L_SHIFT:
                case OT_R_SHIFT:
                    fprintf(file, "%s\n", ot_type_to_mnem[type]);
                    break;
                case OT_ERR: printf("Unexpected operation tree node\n");
            }
        }
    ot = ot->next;
    }
}

void generate_cfg(cfg_block_t* cfg, FILE* file, bool* visited, file_defs_t* defs, struct funcs_queue** queue) {
    if (visited[cfg->id]) {
        fprintf(file, "JMP .%s_%d\n", cfg->label, cfg->id);
        return;
    }
    visited[cfg->id] = true;
    switch (parse_cfg_type(cfg->label)) {
        case CFG_after_if:
        case CFG_after_while:
        case CFG_do_break:
        case CFG_do_start:
            fprintf(file, ".%s_%d:\n", cfg->label, cfg->id);
            generate_cfg(cfg->childs[0], file, visited, defs, queue);
            break;
        case CFG_WHILE:
            fprintf(file, ".%s_%d:\n", cfg->label, cfg->id);
            generate_expr(cfg->operation_tree, cfg->symb_table, file, defs, queue);
            fprintf(file, "JZ .%s_%d\n", cfg->childs[1]->label, cfg->childs[1]->id);
            generate_cfg(cfg->childs[0], file, visited, defs, queue);
            generate_cfg(cfg->childs[1], file, visited, defs, queue);
            break;
        case CFG_do_while:
            generate_expr(cfg->operation_tree, cfg->symb_table, file, defs, queue);
            fprintf(file, "JNZ .%s_%d\n", cfg->childs[0]->label, cfg->childs[0]->id);
            generate_cfg(cfg->childs[1], file, visited, defs, queue);
            break;
        case CFG_IF:
            generate_expr(cfg->operation_tree, cfg->symb_table, file, defs, queue);
            fprintf(file, "JNZ .then_%s_%d\n", cfg->childs[0]->label, cfg->childs[0]->id);
            generate_cfg(cfg->childs[1], file, visited, defs, queue);
    
            fprintf(file, ".then_%s_%d:\n", cfg->childs[0]->label, cfg->childs[0]->id);
            
            generate_cfg(cfg->childs[0], file, visited, defs, queue);
            break;
        case CFG_EXPRESSION:
            generate_expr(cfg->operation_tree, cfg->symb_table, file, defs, queue);
            generate_cfg(cfg->childs[0], file, visited, defs, queue);
            break;
        case CFG_BLOCK_END:
        case CFG_START:
            generate_cfg(cfg->childs[0], file, visited, defs, queue);
            break;
        case CFG_END:
            fprintf(file, "RET\n");
            return;
        default:
            printf("Unexpected cfg block\n");
            break;
    }
}

void generate_func(char* name, file_defs_t* defs, FILE* file, struct funcs_queue** queue) {
    func_sig_t* sig = find_signature_by_name(name, defs->signatures);
    if (!sig) {
        printf("No such function `%s`\n", name);
        *queue = queue_pop(*queue);
        return;
    }
    if (sig->translated) {
        *queue = queue_pop(*queue);
        return;
    }
    if (!sig->defined) {
        printf("Function %s declared, but not defined\n", name);
        *queue = queue_pop(*queue);
        return;
    }
    

    sig->translated = true;
    fprintf(file, "%s:\n", name);
    *queue = queue_pop(*queue);

    // generate_fun_prolog();
    bool* visited = malloc(sizeof(bool)*1024);
    memset(visited, 0, 1024);
    generate_cfg(sig->cfg, file, visited, defs, queue);
    fprintf(file, "\n\n");
    // generate_fun_epilog();
}

void generate_consts(const_table_t* consts, FILE* file) {
    fprintf(file, "[section constants]\n\n");
    while (consts) {
        switch (consts->type->builtin) {
            case MY_BOOL:
                fprintf(file, "const_%d: %s %d", consts->id, "dw", strcmp(consts->literal, "true") == 0);
                fprintf(file, "\t\t\t;\t%s\n", consts->literal);
                break;
            case MY_CHAR:
                fprintf(file, "const_%d: %s %d", consts->id, "dw", consts->literal[1]);
                fprintf(file, "\t\t\t;\t%s\n", consts->literal);
                break;
            case MY_INT:
                fprintf(file, "const_%d: %s %s", consts->id, "dw", consts->literal);
                fprintf(file, "\t\t\t;\t%s\n", consts->literal);
                break;
            case MY_STRING:
                fprintf(file, "const_%d: dw str_%d\n", consts->id, consts->id);
                fprintf(file, "str_%d: %s %d", consts->id, "db", consts->literal[1]);
                for (uint32_t i = 2; i < strlen(consts->literal) - 1; ++i){
                    fprintf(file, ", %d", consts->literal[i]);
                }
                    fprintf(file, ", 0");

                fprintf(file, "\t\t\t;\t%s\n", consts->literal);
                break;
            default:
                printf("Unexpected literal type for `%s`\n", consts->literal);

        }
        consts = consts->next;
    }
    fprintf(file, "\n");
}

void generate_std(file_defs_t* defs, FILE* file) {
    func_sig_t* sig = find_signature_by_name("put_char", defs->signatures);
    if (sig) {
        sig->translated = true;

        fprintf(file, "put_char:\n");
        fprintf(file, "load_data 0\t\t;\tc\n");
        fprintf(file, "OUT\n");
        fprintf(file, "RET\n\n");
    }

    sig = find_signature_by_name("get_char", defs->signatures);
    if (sig) {
        sig->translated = true;

        fprintf(file, "get_char:\n");
        fprintf(file, "IN\n");
        fprintf(file, "RET\n\n");
    }

}

void generate_code(file_defs_t* defs, char* path) {
    struct funcs_queue* queue = queue_push("main", NULL);
    FILE *listing = fopen(path, "w");
    setvbuf(listing, NULL, _IONBF, 10);
    generate_consts(defs->const_table, listing);

    fprintf(listing, "[section code]\n\n");

    fprintf(listing, "CALL main 0\n");
    fprintf(listing, "HLT\n");

    fprintf(listing, "\n");

    generate_std(defs, listing);

    while(queue) {
        generate_func(queue->name, defs, listing, &queue);
        fprintf(listing, "\n");
    }

}