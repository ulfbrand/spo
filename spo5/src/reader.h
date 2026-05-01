#ifndef READER_H
#define READER_H

#include "ast.h"
#include <stdint.h>

#define MAX_PATH 1024

ast_t** process_files(char * input_path, char* output_path, uint32_t* cnt);

#endif
