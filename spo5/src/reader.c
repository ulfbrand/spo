#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "reader.h"
#include "ast.h"
#include "parser.h"

ast_t** process_files(char * input_path, char* output_path, uint32_t* cnt) {
    struct dirent *entry;
    struct stat statbuf;
    DIR *dp;

    dp = opendir(input_path);
    if (dp == NULL) {
        perror("opendir");
        return NULL;
    }

    *cnt = 0;
    while ((entry = readdir(dp)) != NULL) {
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", input_path, entry->d_name);
        stat(full_path, &statbuf);
        if (S_ISREG(statbuf.st_mode)) {
            (*cnt)++;
        }
    }

    ast_t** asts = malloc(sizeof(ast_t*) * (*cnt));
    if (!asts) {
        perror("malloc");
        closedir(dp);
        return NULL;
    }

    rewinddir(dp);

    int i = 0;
    while ((entry = readdir(dp)) != NULL) {
        char input_file[MAX_PATH];
        char ast_dot_file[MAX_PATH];
        snprintf(input_file, MAX_PATH, "%s/%s", input_path, entry->d_name);
        snprintf(ast_dot_file, MAX_PATH, "%s/%s.dot", output_path, entry->d_name);
        stat(input_file, &statbuf);

        if (S_ISREG(statbuf.st_mode)) {
            asts[i] = parse_file(input_file, ast_dot_file);
            i++;
        }
    }

    closedir(dp);
    return asts;
}