#include "ast.h"
#include <stdlib.h>
#include <string.h>

ast_t* create_node(uint32_t children_cnt, char* label, uint32_t line, uint32_t pos){
    ast_t* node = malloc(sizeof(ast_t));
    node->children_cnt = children_cnt;
    node->children = malloc(sizeof(ast_t*) * children_cnt);
    node->label = strdup(label);
    node->line = line;
    node->pos = pos;

    return node;
}

void free_ast(ast_t* ast) {
    if(!ast) return;

    for (uint32_t i = 0; i < ast->children_cnt; ++i) {
        free_ast(ast->children[i]);
    }
    if(ast->children_cnt) {
        free(ast->children);
    }
    if(ast->label) {
        free(ast->label);
    }
    free(ast);
}


ast_t* copy_from_antlr(pANTLR3_BASE_TREE tree) {
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    pANTLR3_UINT8 label = tok->getText(tok)->chars;

    ast_t* node = create_node(tree->getChildCount(tree), label, tok->getLine(tok), tok->getCharPositionInLine(tok));
    if (strcmp(node->label, "NAME") == 0) {
        pANTLR3_BASE_TREE child = tree->getChild(tree, 0);
        pANTLR3_COMMON_TOKEN c_tok = child->getToken(child);
        node->line = c_tok->getLine(c_tok);
        node->pos = c_tok->getCharPositionInLine(c_tok);
    }
    if (strcmp(node->label, "ACCESS") == 0) {
        pANTLR3_BASE_TREE c1 = tree->getChild(tree, 0);
        pANTLR3_BASE_TREE c2 = c1->getChild(c1, 0);
        pANTLR3_COMMON_TOKEN c_tok = c2->getToken(c2);
        node->line = c_tok->getLine(c_tok);
        node->pos = c_tok->getCharPositionInLine(c_tok);
    }

    for (uint32_t i = 0; i < node->children_cnt; ++i) {
        node->children[i] = copy_from_antlr(tree->getChild(tree, i));
    }
    return node;
}

char* fix_label(char* label) {
    char* new_label = malloc(strlen(label)*2);
    char* ptr = new_label;
    while(*label) {
        if(*label == '"') {
            *ptr++ = '\\';
        }
        *ptr++ = *label++;
    }
    *ptr++ = '\0';
    return new_label;
}

uint32_t print_dot(ast_t* node, FILE* file, uint32_t id) {
    char* fixed_label = fix_label(node->label);
    fprintf(file, "n%d[label=\"%s\"]\n", id, fixed_label);
    free(fixed_label);
    uint32_t next_id = id + 1;
    for(uint32_t i = 0; i < node->children_cnt; ++i) {
        fprintf(file, "n%d -> n%d;\n", id, next_id);
        next_id = print_dot(node->children[i], file, next_id);
    }
    return next_id;
}
void ast_to_dot(ast_t* ast, const char* ast_dot_file) {
    FILE *file = fopen(ast_dot_file, "w");
    if(file == NULL) {
        fprintf(stderr, "Error when opening a ast to dot file\n");
    }

    fprintf(file, "digraph AST {\n");
    fprintf(file, "graph [splines=true];\n");
    print_dot(ast, file, 0);

    fprintf(file,"}\n");
    fclose(file);
}