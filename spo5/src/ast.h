#ifndef AST_H
#define AST_H

#include <antlr3.h>
#include <stdint.h>

typedef struct ast{
    uint32_t children_cnt;
    struct ast **children;

    char* label;
    uint32_t line;
    uint32_t pos;
    char* filename;
} ast_t;


ast_t* create_node(uint32_t children_cnt, char* label, uint32_t line, uint32_t pos);

void free_ast(ast_t* ast);

ast_t* copy_from_antlr(pANTLR3_BASE_TREE tree);

void ast_to_dot(ast_t* ast, const char* ast_dot_file);

char* fix_label(char* label);
#endif