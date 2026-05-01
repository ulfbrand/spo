#ifndef CFG_H
#define CFG_H

#include <stdint.h>
#include <stdbool.h>
#include "ast.h"
#include "err_list.h"
#include "ot.h"
#include "tables.h"

struct ot;
typedef struct cfg_block {
    uint32_t id;
    struct ot* operation_tree;
    symb_table_t* symb_table;
    char* label;
    uint32_t child_cnt;

    struct cfg_block** childs;
} cfg_block_t;

typedef enum {
    CFG_after_if,
    CFG_IF,
    CFG_WHILE,
    CFG_after_while,
    CFG_do_break,
    CFG_do_while,
    CFG_do_start,
    CFG_EXPRESSION,
    CFG_BLOCK_END,
    CFG_START,
    CFG_END,
    CFG_ERR,
} cfg_type_e;

typedef enum {
    S_VAR_DEF,
    S_IF_DEF,
    S_BLOCK_DEF,
    S_WHILE_DEF,
    S_DO_DEF,
    S_BREAK_KEYWORD,
    S_USELESS,
    S_EXPRESSION
} statement_type_e;

typedef struct file_defs {
    uint32_t last_id;
    func_sig_t* signatures;
    const_table_t* const_table;
    user_type_t* user_types;
    err_list_t* err;
} file_defs_t;

cfg_type_e parse_cfg_type(char* label);

file_defs_t* parse_ast(ast_t** ast, uint32_t cnt, char* output_path, bool need_ot);
cfg_block_t* parse_block(ast_t* ast, bool is_loop, bool* breaked, cfg_block_t* prev_block, cfg_block_t* end_block, file_defs_t* defs, char* filename);
cfg_block_t* parse_statement(ast_t* ast, bool is_loop, bool *breaked, cfg_block_t* prev_block, cfg_block_t* end_block, file_defs_t* defs, char* filename);

void defs_to_dot(file_defs_t* defs, char* cfg_file_name, bool need_ot);

file_defs_t* create_file_defs();
cfg_block_t* create_cfg(uint32_t id, char* label, uint32_t child_cnt, symb_table_t* symb_table);
void free_cfg(cfg_block_t* cfg);
void free_file_defs(file_defs_t* defs);
#endif
