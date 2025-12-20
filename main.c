/*
 * main.c - shardjs interpreter entry point
 * 
 * reads js file, tokenizes, parses, and executes it.
 * handles errors and cleans up resources.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/token.h"
#include "include/runtime.h"

// read entire file into memory
char* read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }
    
    // get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // read it all
    char *content = malloc(size + 1);
    if (!content) {
        fprintf(stderr, "Error: Could not allocate memory for file content\n");
        fclose(file);
        return NULL;
    }
    
    size_t bytes_read = fread(content, 1, size, file);
    content[bytes_read] = '\0';
    fclose(file);
    
    return content;
}

// cleanup everything in one place
void cleanup_resources(char *source, Lexer *lexer, Parser *parser, ASTNode *ast, Environment *env) {
    if (env) env_destroy(env);
    if (ast) ast_destroy(ast);
    if (parser) parser_destroy(parser);
    if (lexer) lexer_destroy(lexer);
    if (source) free(source);
}

int main(int argc, char *argv[]) {
    // need exactly one arg - the js file
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <script.js>\n", argv[0]);
        fprintf(stderr, "  script.js: Path to JavaScript file to execute\n");
        return 1;
    }
    
    if (strlen(argv[1]) == 0) {
        fprintf(stderr, "Error: Script filename cannot be empty\n");
        return 1;
    }
    
    // init everything to null for safe cleanup
    char *source = NULL;
    Lexer *lexer = NULL;
    Parser *parser = NULL;
    ASTNode *ast = NULL;
    Environment *env = NULL;
    int exit_code = 0;
    
    // read the source file
    source = read_file(argv[1]);
    if (!source) {
        exit_code = 1;
        goto cleanup;
    }
    
    // create lexer
    lexer = lexer_create(source);
    if (!lexer) {
        fprintf(stderr, "Error: Could not create lexer - out of memory\n");
        exit_code = 1;
        goto cleanup;
    }
    
    // create parser
    parser = parser_create(lexer);
    if (!parser) {
        fprintf(stderr, "Error: Could not create parser - out of memory\n");
        exit_code = 1;
        goto cleanup;
    }
    
    // parse into ast
    ast = parser_parse(parser);
    if (!ast) {
        if (parser_has_error(parser)) {
            fprintf(stderr, "Parse error: %s\n", parser_get_error(parser));
        } else {
            fprintf(stderr, "Parse error: Unknown parsing failure\n");
        }
        exit_code = 1;
        goto cleanup;
    }
    
    if (parser_has_error(parser)) {
        fprintf(stderr, "Parse error: %s\n", parser_get_error(parser));
        exit_code = 1;
        goto cleanup;
    }
    
    // create var environment
    env = env_create();
    if (!env) {
        fprintf(stderr, "Error: Could not create environment - out of memory\n");
        exit_code = 1;
        goto cleanup;
    }
    
    // run the code
    interpreter_clear_error();
    double result = interpret(ast, env);
    
    if (interpreter_has_error()) {
        fprintf(stderr, "Runtime error: %s\n", interpreter_get_error());
        exit_code = 1;
        goto cleanup;
    }
    
    // avoid unused var warning
    (void)result;
    
cleanup:
    cleanup_resources(source, lexer, parser, ast, env);
    
    return exit_code;
}