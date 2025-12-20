/*
 * test_parser.c - tests for the recursive descent parser
 * 
 * tests parsing of expressions, statements, error handling,
 * and ast generation from token streams.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/runtime.h"

// helper to print ast nodes
void print_ast(ASTNode *node, int indent) {
    if (!node) {
        printf("%*sNULL\n", indent, "");
        return;
    }
    
    switch (node->type) {
        case AST_NUMBER:
            printf("%*sNUMBER: %g\n", indent, "", node->data.number);
            break;
        case AST_IDENTIFIER:
            printf("%*sIDENTIFIER: %s\n", indent, "", node->data.identifier);
            break;
        case AST_BINARY_OP:
            printf("%*sBINARY_OP: %c\n", indent, "", node->data.binary.operator);
            print_ast(node->data.binary.left, indent + 2);
            print_ast(node->data.binary.right, indent + 2);
            break;
        case AST_LET_DECL:
            printf("%*sLET_DECL: %s\n", indent, "", node->data.let_decl.name);
            print_ast(node->data.let_decl.value, indent + 2);
            break;
        case AST_PRINT_CALL:
            printf("%*sPRINT_CALL:\n", indent, "");
            print_ast(node->data.print_arg, indent + 2);
            break;
        case AST_PROGRAM:
            printf("%*sPROGRAM (%d statements):\n", indent, "", node->data.program.count);
            for (int i = 0; i < node->data.program.count; i++) {
                printf("%*sStatement %d:\n", indent + 2, "", i);
                print_ast(node->data.program.statements[i], indent + 4);
            }
            break;
    }
}

// test parsing a simple expression
void test_expression_parsing() {
    printf("Testing expression parsing...\n");
    
    const char *source = "2 + 3 * 4";
    Lexer *lexer = lexer_create(source);
    Parser *parser = parser_create(lexer);
    
    ASTNode *ast = parser_parse(parser);
    
    if (parser_has_error(parser)) {
        printf("Parser error: %s\n", parser_get_error(parser));
        assert(0);
    }
    
    assert(ast != NULL);
    assert(ast->type == AST_PROGRAM);
    assert(ast->data.program.count == 1);
    assert(ast->data.program.statements[0]->type == AST_BINARY_OP);
    assert(ast->data.program.statements[0]->data.binary.operator == '+');
    
    printf("Expression AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Expression parsing test passed!\n\n");
}

// test parsing a let declaration
void test_let_declaration() {
    printf("Testing let declaration parsing...\n");
    
    const char *source = "let x = 42;";
    Lexer *lexer = lexer_create(source);
    Parser *parser = parser_create(lexer);
    
    ASTNode *ast = parser_parse(parser);
    
    if (parser_has_error(parser)) {
        printf("Parser error: %s\n", parser_get_error(parser));
        assert(0);
    }
    
    assert(ast != NULL);
    assert(ast->type == AST_PROGRAM);
    assert(ast->data.program.count == 1);
    assert(ast->data.program.statements[0]->type == AST_LET_DECL);
    assert(strcmp(ast->data.program.statements[0]->data.let_decl.name, "x") == 0);
    
    printf("Let declaration AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Let declaration test passed!\n\n");
}

// test parsing a print call
void test_print_call() {
    printf("Testing print call parsing...\n");
    
    const char *source = "print(123);";
    Lexer *lexer = lexer_create(source);
    Parser *parser = parser_create(lexer);
    
    ASTNode *ast = parser_parse(parser);
    
    if (parser_has_error(parser)) {
        printf("Parser error: %s\n", parser_get_error(parser));
        assert(0);
    }
    
    assert(ast != NULL);
    assert(ast->type == AST_PROGRAM);
    assert(ast->data.program.count == 1);
    assert(ast->data.program.statements[0]->type == AST_PRINT_CALL);
    
    printf("Print call AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Print call test passed!\n\n");
}

// test error handling
void test_error_handling() {
    printf("Testing error handling...\n");
    
    // test with missing identifier after let
    const char *source = "let = 42;";
    Lexer *lexer = lexer_create(source);
    Parser *parser = parser_create(lexer);
    
    ASTNode *ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("First error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // test with missing expression after =
    const char *bad_source = "let x = ;";
    lexer = lexer_create(bad_source);
    parser = parser_create(lexer);
    
    ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("Second error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Error handling test passed!\n\n");
}

int main() {
    printf("Running parser tests...\n\n");
    
    test_expression_parsing();
    test_let_declaration();
    test_print_call();
    test_error_handling();
    
    printf("All parser tests passed!\n");
    return 0;
}