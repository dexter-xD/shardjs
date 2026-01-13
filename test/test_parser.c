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

// test comparison operator precedence
void test_comparison_precedence() {
    printf("Testing comparison operator precedence...\n");
    
    // Test that "5 + 3 > 2 * 4" parses as "(5 + 3) > (2 * 4)"
    const char *source = "5 + 3 > 2 * 4";
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
    
    ASTNode *expr = ast->data.program.statements[0];
    assert(expr->type == AST_BINARY_OP);
    assert(expr->data.binary.operator == '>');
    
    // Left side should be "5 + 3"
    ASTNode *left = expr->data.binary.left;
    assert(left->type == AST_BINARY_OP);
    assert(left->data.binary.operator == '+');
    assert(left->data.binary.left->type == AST_NUMBER);
    assert(left->data.binary.left->data.number == 5.0);
    assert(left->data.binary.right->type == AST_NUMBER);
    assert(left->data.binary.right->data.number == 3.0);
    
    // Right side should be "2 * 4"
    ASTNode *right = expr->data.binary.right;
    assert(right->type == AST_BINARY_OP);
    assert(right->data.binary.operator == '*');
    assert(right->data.binary.left->type == AST_NUMBER);
    assert(right->data.binary.left->data.number == 2.0);
    assert(right->data.binary.right->type == AST_NUMBER);
    assert(right->data.binary.right->data.number == 4.0);
    
    printf("Precedence test AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Comparison precedence test passed!\n\n");
}

// test comparison operator associativity
void test_comparison_associativity() {
    printf("Testing comparison operator associativity...\n");
    
    // Test that "1 < 2 < 3" parses as "(1 < 2) < 3" (left-to-right)
    const char *source = "1 < 2 < 3";
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
    
    ASTNode *expr = ast->data.program.statements[0];
    assert(expr->type == AST_BINARY_OP);
    assert(expr->data.binary.operator == '<');
    
    // Left side should be "(1 < 2)"
    ASTNode *left = expr->data.binary.left;
    assert(left->type == AST_BINARY_OP);
    assert(left->data.binary.operator == '<');
    assert(left->data.binary.left->type == AST_NUMBER);
    assert(left->data.binary.left->data.number == 1.0);
    assert(left->data.binary.right->type == AST_NUMBER);
    assert(left->data.binary.right->data.number == 2.0);
    
    // Right side should be "3"
    ASTNode *right = expr->data.binary.right;
    assert(right->type == AST_NUMBER);
    assert(right->data.number == 3.0);
    
    printf("Associativity test AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Comparison associativity test passed!\n\n");
}

// test mixed arithmetic and comparison operators
void test_mixed_operators() {
    printf("Testing mixed arithmetic and comparison operators...\n");
    
    // Test that "10 - 5 >= 2 + 3" parses as "(10 - 5) >= (2 + 3)"
    const char *source = "10 - 5 >= 2 + 3";
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
    
    ASTNode *expr = ast->data.program.statements[0];
    assert(expr->type == AST_BINARY_OP);
    assert(expr->data.binary.operator == 'G'); // >= encoded as 'G'
    
    // Left side should be "10 - 5"
    ASTNode *left = expr->data.binary.left;
    assert(left->type == AST_BINARY_OP);
    assert(left->data.binary.operator == '-');
    
    // Right side should be "2 + 3"
    ASTNode *right = expr->data.binary.right;
    assert(right->type == AST_BINARY_OP);
    assert(right->data.binary.operator == '+');
    
    printf("Mixed operators test AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Mixed operators test passed!\n\n");
}

// test all comparison operators
void test_all_comparison_operators() {
    printf("Testing all comparison operators...\n");
    
    const char *operators[] = {">", "<", ">=", "<=", "==", "!="};
    const char expected_chars[] = {'>', '<', 'G', 'L', 'E', 'N'};
    
    for (int i = 0; i < 6; i++) {
        char source[20];
        snprintf(source, sizeof(source), "5 %s 3", operators[i]);
        
        Lexer *lexer = lexer_create(source);
        Parser *parser = parser_create(lexer);
        
        ASTNode *ast = parser_parse(parser);
        
        if (parser_has_error(parser)) {
            printf("Parser error for %s: %s\n", operators[i], parser_get_error(parser));
            assert(0);
        }
        
        assert(ast != NULL);
        assert(ast->type == AST_PROGRAM);
        assert(ast->data.program.count == 1);
        
        ASTNode *expr = ast->data.program.statements[0];
        assert(expr->type == AST_BINARY_OP);
        assert(expr->data.binary.operator == expected_chars[i]);
        
        printf("Operator %s parsed correctly as '%c'\n", operators[i], expected_chars[i]);
        
        ast_destroy(ast);
        parser_destroy(parser);
        lexer_destroy(lexer);
    }
    
    printf("All comparison operators test passed!\n\n");
}

// test comparison operator error handling
void test_comparison_error_handling() {
    printf("Testing comparison operator error handling...\n");
    
    // Test incomplete comparison expression
    const char *source = "5 >";
    Lexer *lexer = lexer_create(source);
    Parser *parser = parser_create(lexer);
    
    ASTNode *ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("Incomplete comparison error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test comparison with missing right operand
    const char *source2 = "let x = 5 >=;";
    lexer = lexer_create(source2);
    parser = parser_create(lexer);
    
    ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("Missing right operand error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test comparison in invalid context
    const char *source3 = "let = 5 > 3;";
    lexer = lexer_create(source3);
    parser = parser_create(lexer);
    
    ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("Invalid context error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test nested comparison error propagation
    const char *source4 = "print((5 + ) >= 10);";
    lexer = lexer_create(source4);
    parser = parser_create(lexer);
    
    ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("Nested comparison error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Comparison operator error handling test passed!\n\n");
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
    test_comparison_precedence();
    test_comparison_associativity();
    test_mixed_operators();
    test_all_comparison_operators();
    test_comparison_error_handling();
    test_error_handling();
    
    printf("All parser tests passed!\n");
    return 0;
}