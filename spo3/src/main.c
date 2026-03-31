#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "reader.h"
#include "ast.h"
#include "cfg.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s <input_path> <ast dot path> <cfg path> <need print ot (1|0)> <listing path>\n", argv[0]);
        return -1;
    }
    if (strlen(argv[4]) != 1 || (argv[4][0] != '1' && argv[4][0] != '0')) {
        printf("Last param must be '1' or '0', but got %s\n", argv[4]);
        return -1;
    }

    bool need_ot = argv[4][0] - '0';

    uint32_t files_cnt;
    ast_t** files_ast = process_files(argv[1], argv[2], &files_cnt);

    file_defs_t* defs = parse_ast(files_ast, files_cnt, argv[3], need_ot);

    print_errors(defs->err);


    generate_code(defs, argv[5]);


    //free asts
    //free defs

    return 0;
}
