#include <antlr3defs.h>
#include <antlr3interfaces.h>
#include <Lab1GrammarLexer.h>
#include <Lab1GrammarParser.h>
#include "parser.h"
#include "cfg.h"
#include "err_list.h"

ast_t* parse_file(char *input_file, char* ast_dot_file){
    pANTLR3_INPUT_STREAM input = antlr3FileStreamNew((pANTLR3_UINT8)input_file, ANTLR3_ENC_8BIT);

    if (input == NULL) {
        fprintf(stderr, "Error opening the input file\n");
        return NULL;
    }

    pLab1GrammarLexer lexer = Lab1GrammarLexerNew(input);

    if (lexer == NULL) {
        fprintf(stderr, "Error creating the lexer\n");
        input->close(input);
        return NULL;
    }

    pANTLR3_COMMON_TOKEN_STREAM tokenStream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));

    if (tokenStream == NULL) {
        fprintf(stderr, "Error when creating a token stream\n");
        lexer->free(lexer);
        input->close(input);
        return NULL;
    }

    pLab1GrammarParser parser = Lab1GrammarParserNew(tokenStream);

    if (parser == NULL) {
        fprintf(stderr, "Error creating the parser\n");
        tokenStream->free(tokenStream);
        lexer->free(lexer);
        input->close(input);
        return NULL;
    }

    Lab1GrammarParser_source_return result = parser->source(parser);

    if (result.tree == NULL) {
        fprintf(stderr, "Error creating the tree\n");
        tokenStream->free(tokenStream);
        lexer->free(lexer);
        parser->free(parser);
        input->close(input);
        return NULL;
    }

    pANTLR3_BASE_TREE ast = (pANTLR3_BASE_TREE)result.tree;

    ast_t* my_ast = NULL;
    if (ast != NULL) {
        my_ast = copy_from_antlr(ast);
        my_ast->filename = strdup(input_file);
        ast_to_dot(my_ast, ast_dot_file);
    }

    parser->free(parser);
    tokenStream->free(tokenStream);
    lexer->free(lexer);
    input->close(input);

    return my_ast;
}
