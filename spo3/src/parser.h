#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

ast_t* parse_file(char *input_file, char* ast_dot_file);

#endif
