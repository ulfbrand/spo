#ifndef OT_H
#define OT_H

#include <stdint.h>
#include "ast.h"
#include "type.h"

struct type;
typedef struct ot {
    uint32_t line;
    uint32_t pos;
    char* label;
    uint32_t child_cnt;
    struct ot** childs;
    union {
        struct type* type;
        char* name;

    };
    struct ot* next;
} ot_t;


ot_t* create_ot(uint32_t line, uint32_t pos, char* label, uint32_t childs_cnt);
void free_ot(ot_t*ot);

struct file_defs;
struct cfg_block;
ot_t* ast_to_ot(ast_t* ast, struct file_defs* defs, struct cfg_block* block);
void ot_to_dot(ot_t* ot, uint32_t id, uint32_t *available_id, FILE* file);

#endif