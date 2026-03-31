#include "cfg.h"
#include "err_list.h"
#include <string.h>
#include <stdbool.h>
#include "reader.h"

cfg_block_t* create_cfg(uint32_t id, char* label, uint32_t child_cnt, symb_table_t* sym_table){
    cfg_block_t* cfg = malloc(sizeof(cfg_block_t));
    cfg->id = id;
    cfg->label = strdup(label);
    cfg->child_cnt = child_cnt;
    cfg->childs = malloc(sizeof(cfg_block_t*)*child_cnt);
    memset(cfg->childs, 0, sizeof(cfg_block_t*)*child_cnt);
    cfg->operation_tree = NULL;
    cfg->symb_table = sym_table;
    return cfg;
}

file_defs_t* create_file_defs() {
    file_defs_t* defs = malloc(sizeof(file_defs_t));
    defs->last_id = 0;
    defs->signatures = NULL;
    defs->err = NULL;
    defs->const_table = NULL;
    return defs;
}

void free_cfg(cfg_block_t* cfg) {
    if(!cfg) return;
    free_ot(cfg->operation_tree);
    if(cfg->label) free(cfg->label);
    for(uint32_t i = 0; i < cfg->child_cnt; ++i){
        free_cfg(cfg->childs[i]);
    }
    if(cfg->child_cnt) free(cfg->childs);
    free(cfg);
}

void free_file_defs(file_defs_t* defs) {
    if(!defs) return;
    free(defs);
}

statement_type_e parse_statement_type(char* label) {
    if(strcmp(label, "VAR_DEF") == 0) return S_VAR_DEF;
    if(strcmp(label, "IF_DEF") == 0) return S_IF_DEF;
    if(strcmp(label, "BLOCK_DEF") == 0) return S_BLOCK_DEF;
    if(strcmp(label, "WHILE_DEF") == 0) return S_WHILE_DEF;
    if(strcmp(label, "DO_DEF") == 0) return S_DO_DEF;
    if(strcmp(label, "break") == 0) return S_BREAK_KEYWORD;
    if(strcmp(label, "EXPRESSION") == 0) return S_EXPRESSION;
    return S_USELESS;
}

cfg_type_e parse_cfg_type(char* label) {
    if(strcmp(label, "after_if") == 0) return CFG_after_if;
    if(strcmp(label, "IF") == 0) return CFG_IF;
    if(strcmp(label, "WHILE") == 0) return CFG_WHILE;
    if(strcmp(label, "after_while") == 0) return CFG_after_while;
    if(strcmp(label, "do_break") == 0) return CFG_do_break;
    if(strcmp(label, "do_while") == 0) return CFG_do_while;
    if(strcmp(label, "do_start") == 0) return CFG_do_start;
    if(strcmp(label, "EXPRESSION") == 0) return CFG_EXPRESSION;
    if(strcmp(label, "BLOCK_END") == 0) return CFG_BLOCK_END;
    if(strcmp(label, "START") == 0) return CFG_START;
    if(strcmp(label, "END") == 0) return CFG_END;
    return CFG_ERR;
}

cfg_block_t* parse_if(ast_t* ast, bool is_loop, bool* breaked, cfg_block_t* prev_block, cfg_block_t* end_block, file_defs_t* defs, char* filename) {
    cfg_block_t* last_block;
    cfg_block_t* if_block;
    cfg_block_t* empty = create_cfg(defs->last_id++, "after_if", 1, prev_block->symb_table);
    uint32_t if_id = defs->last_id++;
    uint32_t break_cnt = 2;
    bool curr_break;

    prev_block->childs[0] = create_cfg(if_id, "IF",  2, prev_block->symb_table);
    if_block = prev_block->childs[0];
    if_block->operation_tree = ast_to_ot(ast->children[0]->children[0], defs, if_block);
    if(ast->children_cnt == 3) {
        
        curr_break = false;
        last_block = parse_statement(ast->children[2]->children[0], is_loop, &curr_break, if_block, end_block, defs, filename);
        if (last_block != end_block) {
            break_cnt--;
            last_block->childs[0] = empty;
        }
        if_block->childs[1] = if_block->childs[0];
        if_block->id = empty->id;
    } else {
        if_block->childs[1] = empty;
        break_cnt--;
    }

    curr_break = false;
    last_block = parse_statement(ast->children[1], is_loop, &curr_break, if_block, end_block, defs, filename);
    if_block->id = if_id;

    if (last_block != end_block) {
        break_cnt--;
        last_block->childs[0] = empty;
    }

    if_block->id = if_id;

    if(break_cnt == 2) {
        *breaked = true;
        free_cfg(empty);
        return end_block;
    }
    
    return empty;
}

cfg_block_t* parse_while(ast_t* ast, bool is_loop, cfg_block_t* prev_block, cfg_block_t* end_block, file_defs_t* defs, char* filename) {
    prev_block->childs[0] = create_cfg(defs->last_id++, "WHILE", 2, prev_block->symb_table);
    cfg_block_t* while_block = prev_block->childs[0];

    while_block->operation_tree = ast_to_ot(ast->children[0]->children[0], defs, while_block);
    cfg_block_t* break_block = create_cfg(defs->last_id++, "after_while", 1, prev_block->symb_table);

    bool breaked = false;
    cfg_block_t* last_block = parse_statement(ast->children[1], true, &breaked, while_block, break_block, defs, filename);
    if (last_block != break_block) {
        last_block->childs[0] = while_block;
    }
    while_block->childs[1] = break_block;

    return break_block;
}

cfg_block_t* parse_do(ast_t* ast, bool is_loop, cfg_block_t* prev_block, cfg_block_t* end_block, file_defs_t* defs, char* filename) {

    prev_block->childs[0] = create_cfg(defs->last_id++, "do_start", 1, prev_block->symb_table);
    cfg_block_t* while_block = prev_block->childs[0];


    cfg_block_t* break_block = create_cfg(defs->last_id++, "do_break", 1, prev_block->symb_table);

    bool breaked = false;
    cfg_block_t* last_block = parse_block(ast->children[0]->children[0], true, &breaked, while_block, break_block, defs, filename);

    if(last_block != break_block) {
        cfg_block_t* empty_block = create_cfg(defs->last_id++, "do_while", 2, prev_block->symb_table);
        empty_block->operation_tree = ast_to_ot(ast->children[1]->children[0], defs, empty_block);
        last_block->childs[0] = empty_block;
        empty_block->childs[0] = prev_block->childs[0];
        empty_block->childs[1] = break_block;
        return break_block;
    }
    return break_block;
}

cfg_block_t* parse_expr(ast_t* ast, cfg_block_t* prev_block, file_defs_t* defs) {
    prev_block->childs[0] = create_cfg(defs->last_id++, "EXPRESSION", 1, prev_block->symb_table);
    cfg_block_t* expr_block = prev_block->childs[0];
    expr_block->operation_tree = ast_to_ot(ast, defs, expr_block);

    return expr_block;
}

cfg_block_t* parse_var(ast_t* ast, cfg_block_t* prev_block, file_defs_t* defs) {
    cfg_block_t* last_block = prev_block;

    // type_t* type = parse_type(ast->children[0], defs);
    for (uint32_t i = 1; i <  ast->children_cnt; ++i) {
        //add_to_symb_table(ast->children[i]->children[0], type);
        if (ast->children[i]->children_cnt == 2) {
            last_block = parse_expr(ast->children[i]->children[1], last_block, defs);
        }
    }
    return last_block;
}

cfg_block_t* parse_statement(ast_t* ast, bool is_loop, bool *breaked, cfg_block_t* prev_block, cfg_block_t* end_block, file_defs_t* defs, char* filename) {
    statement_type_e s_type = parse_statement_type(ast->label);
    cfg_block_t* last_block;
    if  (*breaked) {
        push_err(&defs->err, "%s: Statement `%s`, line: %d, pos: %d will not be compiled, after break in %s", WARN, ast->label, ast->line, ast->pos, filename);
        return prev_block;
    } else {
        switch (s_type) {
        case S_VAR_DEF: {
            last_block = parse_var(ast, prev_block, defs);
            break;
        }
        case S_IF_DEF: {
            last_block = parse_if(ast, is_loop, breaked, prev_block, end_block, defs, filename);
            break;
        }
        case S_BLOCK_DEF: {
            last_block = parse_block(ast, is_loop, breaked, prev_block, end_block, defs, filename);
            break;
        }
        case S_WHILE_DEF: {
            last_block = parse_while(ast, true, prev_block, end_block, defs, filename);
            break;
        }
        case S_DO_DEF: {
            last_block = parse_do(ast, true, prev_block, end_block, defs, filename);
            break;
        }
        case S_BREAK_KEYWORD: {
            if (!is_loop) {
                push_err(&defs->err, "%s: Break out of the loop, line: %d, pos: %d in %s", ERROR, ast->line, ast->pos, filename);
                last_block = prev_block;
                break;
            } else {
                *breaked = true;
                if (!end_block) {
                    push_err(&defs->err, "%s: Break to nowhere, line: %d, pos: %d in %s", ERROR, ast->line, ast->pos, filename);
                    break;
                }
                prev_block->childs[0] = end_block;
                last_block = prev_block->childs[0];
                break;
            }
        }
        case S_EXPRESSION: {
            last_block = parse_expr(ast, prev_block, defs);
            break;
        }
        default: {
            push_err(&defs->err, "%s: unknown statement (`%s`, line: %d, pos: %d) in %s", ERROR, ast->label, ast->line, ast->pos, filename);
            break;
        }
        }
    }
    return last_block;
}

cfg_block_t* parse_block(ast_t* ast, bool is_loop, bool* breaked, cfg_block_t* prev_block, cfg_block_t* end_block, file_defs_t* defs, char* filename) {
    if(strcmp(ast->label, "BLOCK_DEF") != 0) {
        push_err(&defs->err, "%s: unknown node `%s` line:%d, pos:%d in %s", ERROR, ast->label, ast->line, ast->pos, filename);
        return NULL;
    }
    cfg_block_t* last_block = prev_block;
    for (uint32_t i = 0; i < ast->children_cnt; ++i){
        ast_t* curr_node = ast->children[i];
        last_block = parse_statement(curr_node, is_loop, breaked, last_block, end_block, defs, filename);

    }
    last_block->childs[0] = create_cfg(defs->last_id++, "BLOCK_END", 1, prev_block->symb_table);
    last_block = last_block->childs[0];
    return last_block;
}

cfg_block_t* parse_body(ast_t* root, file_defs_t* defs, func_sig_t* sig, char* filename) {
    if(strcmp(root->label, "EMPTY_ROOT_DEF") == 0) {
        return NULL;
    }
    if(strcmp(root->label, "ROOT_DEF") != 0) {
        push_err(&defs->err, "%s: Unknown func body in %s", ERROR, filename);
        return NULL;
    }

    cfg_block_t* start_block = create_cfg(defs->last_id++, "START", 1, sig->start_scope);
    cfg_block_t* end_block = create_cfg(defs->last_id++, "END", 0, sig->start_scope);
    
    bool breaked = false;
    cfg_block_t* last_block = parse_block(root->children[0], false, &breaked, start_block, NULL, defs, filename);
    last_block->childs[0] = end_block;
    return start_block;
}

func_sig_t* parse_signature(ast_t* func_def, file_defs_t* defs, char* filename) {
    ast_t* func_sig = func_def->children[0];
    ast_t* func_body = func_def->children[1];
    func_sig_t* signature = malloc(sizeof(func_sig_t));
    signature->cfg = NULL;
    if(strcmp(func_sig->label, "FUNC_SIG") == 0) {
        signature->return_type = parse_type(func_sig->children[0], defs);
        signature->name = strdup(func_sig->children[1]->label);
    } else if (strcmp(func_sig->label, "UNTYPE_FUNC_SIG") == 0){
        signature->return_type = create_non_array_type("VOID");
        signature->name = strdup(func_sig->children[0]->label);
    } else {
        push_err(&defs->err, "%s: Unknown func type in %s", ERROR, filename);
        return NULL;
    }
    signature->args_cnt = 0;
    signature->translated = false;
    ast_t* args = func_sig->children[func_sig->children_cnt - 1];
    signature->args_cnt = args->children_cnt;
    signature->args_types = malloc(sizeof(type_t*)*signature->args_cnt);
    signature->arg_names = malloc(sizeof(char *)*signature->args_cnt);
    symb_table_t* symb_table = NULL;
    for (uint32_t i = 0; i < signature->args_cnt; ++i) {
        if (args->children[i]->children_cnt == 2) {
            signature->args_types[i] = parse_type(args->children[i]->children[0], defs);
            signature->arg_names[i] = strdup(args->children[i]->children[1]->label);
        } else {
            signature->arg_names[i] = strdup(args->children[i]->children[0]->label);
        }
        symb_table = push_var(signature->arg_names[i], symb_table);

    }
    signature->start_scope = symb_table;
    signature->defined = true;
    if (strcmp(func_body->label, "EMPTY_ROOT_DEF") == 0) {
        signature->defined = false;
    }
    return signature;
}

file_defs_t* parse_ast(ast_t** ast, uint32_t cnt, char* output_path, bool need_ot) {
    uint32_t cfg_cnt = 0;
    file_defs_t* defs = create_file_defs();
    for (uint32_t i = 0; i < cnt; ++i) {
        for (uint32_t j = 0; j < ast[i]->children_cnt; ++j) {
            ast_t* func_def = ast[i]->children[j];
            func_sig_t* curr_sig = parse_signature(func_def, defs, ast[i]->filename);
            find_status_e status = find_signature(curr_sig, defs->signatures);

            switch(status) {
                case REDECLARATION:
                    push_err(&defs->err, "%s: Redeclaration of function `%s` in %s", ERROR, curr_sig->name, ast[i]->filename);
                    break;
                case REDEFINITION:
                    push_err(&defs->err, "%s: Redefinition of function `%s` in %s", ERROR, curr_sig->name, ast[i]->filename);
                    break;
                case FOUND:
                    if (!curr_sig->defined) break;
                case NOT_FOUND:
                    curr_sig->next = defs->signatures;
                    defs->signatures = curr_sig;
                    if (curr_sig->defined) {
                        cfg_block_t* body = parse_body(func_def->children[1], defs, curr_sig, ast[i]->filename);
                        curr_sig->cfg = body;
                    }            
                    break;
            }
        }
    }

    defs_to_dot(defs, output_path, need_ot);
    return defs;
}

void print_cfg_dot(cfg_block_t* node, bool* visited, bool need_ot, uint32_t *ot_id, FILE* file) {
    if (!node) return;

    visited[node->id] = true;
    char* fixed_label = fix_label(node->label);
    fprintf(file, "n_%d[label=\"%s\"]\n", node->id, fixed_label);
    if(need_ot) {
        if (node->operation_tree) {
            fprintf(file, "subgraph cluster_%d {\n",node->id);
            uint32_t id = *ot_id;
            (*ot_id)++;
            fprintf(file, "n_%d -> ot_%d[label=ot];\n", node->id, id);
            ot_to_dot(node->operation_tree, id, ot_id, file);
            fprintf(file, "}\n");   
        }
    }
    free(fixed_label);
    for(uint32_t i = 0; i < node->child_cnt; ++i) {
        fprintf(file, "n_%d -> n_%d", node->id, node->childs[i]->id);
        if (node->child_cnt == 2) {
            fprintf(file, "[label=%s, color=%s]", i == 0 ? "True" : "False", i == 0 ? "green" : "red");
        }
        fprintf(file, ";\n");
    }
    
    for(uint32_t i = 0; i < node->child_cnt; ++i) {
        if (!visited[node->childs[i]->id]) {
            print_cfg_dot(node->childs[i], visited, need_ot, ot_id, file);
        }
    }
}

void defs_to_dot(file_defs_t* defs, char* output_path, bool need_ot) {
    func_sig_t* sig = defs->signatures;
    for (; sig; sig = sig->next) {
        cfg_block_t* cfg = sig->cfg;
        if (!cfg) continue;
        char cfg_file_name[MAX_PATH];
        snprintf(cfg_file_name, MAX_PATH, "%s/%s.dot", output_path, sig->name);
        FILE *file = fopen(cfg_file_name, "w");
        setvbuf(file, NULL, _IONBF, 10);
        uint32_t ot_id = 0;
        if(file == NULL) {
            fprintf(stderr, "%s when opening a cfg to dot file\n", ERROR);
        }

        fprintf(file, "digraph CFG {\n");
        fprintf(file, "graph [splines=true];\n");

        bool* visited = malloc(sizeof(bool)*1024);
        memset(visited, 0, 1024);
        print_cfg_dot(cfg, visited, need_ot, &ot_id, file);
        fprintf(file, "}\n");

        free(visited);
        fclose(file);
    }
}
