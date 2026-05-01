#include "ot.h"
#include <stdlib.h>
#include "err_list.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

ot_t* create_ot(uint32_t line, uint32_t pos, char* label, uint32_t childs_cnt){
    ot_t* ot = malloc(sizeof(ot_t));
    ot->line = line;
    ot->pos = pos;
    ot->label = strdup(label);
    ot->child_cnt = childs_cnt;
    ot->next = NULL;
    ot->childs = malloc(sizeof(ot_t*)*childs_cnt);
    memset(ot->childs, 0, sizeof(ot_t*)*childs_cnt);
    return ot;
}

void free_ot(ot_t* ot) {
    if(!ot) return;
    if(ot->label) free(ot->label);
    for(uint32_t i = 0; i < ot->child_cnt; ++i){
        free_ot(ot->childs[i]);
    }
    if(ot->child_cnt) free(ot->childs);
    free(ot);
}

bool is_loadable(ast_t* ast) {
    return (strcmp(ast->label, "NAME") == 0 || (strcmp(ast->label, "ACCESS") == 0 && strcmp(ast->children[1]->label, "INDEXER_DEF") == 0));
}

bool is_op(char* op) {
    switch(op[0]) {
        case '|':
        case '&':
        case '!':
        case '=':
        case '*':
        case '-':
        case '/':
        case '<':
        case '>':
        case '%':
        case '+': return true;
        default: return false;
    }
}

ot_t* expr_to_ot(ast_t* ast, struct file_defs* defs, bool from_load, struct cfg_block* block) {
    ot_t* expr = NULL;
    if (strcmp(ast->label, "EXPRESSION") == 0) {
        expr = expr_to_ot(ast->children[0], defs, true, block);
    } else if(strcmp(ast->label, "ASSIGNMENT") == 0) {
        if (!is_loadable(ast->children[1])) {
            push_err(&defs->err, "%s: Expexted name or array element, but got %s (line %d, pos %d)", ERROR, ast->children[1]->label, ast->children[1]->line, ast->children[1]->pos);
        }
        expr = expr_to_ot(ast->children[1], defs, false, block);

        if (strcmp(ast->children[0]->label, "=") == 0) {
            expr->childs[1] = expr_to_ot(ast->children[2], defs, true, block);
        } else {
            char *action = strdup(ast->children[0]->label);
            action[strlen(action) - 1] = '\0';
            expr->childs[1] = create_ot(ast->children[0]->line, ast->children[0]->pos, action, 2);
            ot_t* assign = expr->childs[1];

            assign->childs[0] = expr_to_ot(ast->children[1], defs, true, block);

            assign->childs[1] = expr_to_ot(ast->children[2], defs, true, block);
        }
    } else if (is_loadable(ast)) {
        if (strcmp(ast->label, "NAME") == 0) {
            if(from_load) {
                symb_table_t* var = get_var(ast->children[0]->label, block->symb_table);
                if (!var) {
                    push_err(&defs->err, "%s: No `%s` variable in scope (line %d, pos %d)", ERROR, ast->children[0]->label, ast->children[0]->line, ast->children[0]->pos);
                }
                expr = create_ot(ast->children[0]->line, ast->children[0]->pos, "load_data", 1);
            } else {
                symb_table_t* var = get_var(ast->children[0]->label, block->symb_table);
                if (!var) {
                    block->symb_table = push_var(ast->children[0]->label, block->symb_table);
                }
                expr = create_ot(ast->children[0]->line, ast->children[0]->pos, "store", 2);
            }
            expr->childs[0] = create_ot(ast->children[0]->line, ast->children[0]->pos, "name", 0);
            expr->childs[0]->name = strdup(ast->children[0]->label);
        } else {

            if(from_load) {
                expr = create_ot(ast->children[0]->line, ast->children[0]->pos, "load_data", 1);
            } else {
                expr = create_ot(ast->children[0]->line, ast->children[0]->pos, "store", 2);
            }

            expr->childs[0] = create_ot(ast->children[0]->children[0]->line, ast->children[0]->children[0]->pos, "ARRAY", 2);
            ot_t* arr = expr->childs[0];
            if (ast->children_cnt != 2) {
                push_err(&defs->err, "%s: Jagged arrays are not supported (line %d, pos %d)", ERROR, ast->children[0]->line, ast->children[0]->pos);
                return expr;
            }

            arr->childs[0] = create_ot(ast->children[0]->line, ast->children[0]->pos, "name", 0);
            arr->childs[0]->name = strdup(ast->children[0]->children[0]->label);

            ot_t** curr_ind = &(arr->childs[1]);
            for (uint32_t i = 0; i < ast->children[1]->children_cnt; ++i) {
                *curr_ind = expr_to_ot(ast->children[1]->children[i], defs, true, block);
                curr_ind = &(*curr_ind)->next;
            }
        }
    } else if (strcmp(ast->label, "ACCESS") == 0) {
        if (strcmp(ast->children[0]->label, "NAME") != 0) {
            push_err(&defs->err, "%s: It must be name of function (line %d, pos %d)", ERROR, ast->children[0]->line, ast->children[0]->pos);
        }
        expr = create_ot(ast->children[0]->children[0]->line, ast->children[0]->children[0]->pos, "call", ast->children[1]->children_cnt > 0 ? 2 : 1);
        expr->childs[0] = create_ot(ast->children[0]->children[0]->line, ast->children[0]->children[0]->pos, "name", 0);
        expr->childs[0]->name = strdup(ast->children[0]->children[0]->label);
        ot_t** curr_arg = &expr->childs[1];
        for(uint32_t i = 0; i < ast->children[1]->children_cnt; ++i) {
            *curr_arg = expr_to_ot(ast->children[1]->children[i], defs, true, block);
            curr_arg = &(*curr_arg)->next;
        }
    } else if (is_op(ast->label)) {
        expr = create_ot(ast->line, ast->pos, ast->label, ast->children_cnt);
        for (uint32_t i = 0; i < ast->children_cnt; ++i) {
            expr->childs[i] = expr_to_ot(ast->children[i], defs, true, block);
        }
    } else {
        expr = create_ot(ast->line, ast->pos, "load_const", 1);
        expr->childs[0] = create_ot(ast->line, ast->pos, "literal", 0);
        expr->childs[0]->name = strdup(ast->label);
        type_t* type = detect_type(ast->label);
        defs->const_table = push_constant(type, ast->label, defs->const_table);
    }
    return expr;
}

ot_t* ast_to_ot(ast_t* ast, struct file_defs* defs, struct cfg_block* block) {
    return expr_to_ot(ast, defs, true, block);
}


void ot_to_dot(ot_t* ot, uint32_t id, uint32_t *available_id, FILE* file) {

    if(!ot) return;
    char* label = (strcmp(ot->label, "literal") == 0 || strcmp(ot->label, "name") == 0) ? ot->name : ot->label;
    label = (strcmp(ot->label, "type") == 0) ? type_to_string(ot->type) : label;
    fprintf(file, "ot_%d[label=\"%s\"]\n", id, fix_label(label));
    if(ot->next) {
        uint32_t next_id = *available_id;
        (*available_id)++;
        fprintf(file, "ot_%d -> ot_%d[label=next];\n", id, next_id);
        ot_to_dot(ot->next, next_id, available_id, file);
    }
    for (uint32_t i = 0; i < ot->child_cnt; ++i) {
        uint32_t next_id = *available_id;
        (*available_id)++;
        fprintf(file, "ot_%d -> ot_%d[label=child%d,color=orange];\n", id, next_id, i);
        ot_to_dot(ot->childs[i], next_id, available_id, file);
    }
}