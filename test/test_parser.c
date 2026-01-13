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
        case AST_IF_STMT:
            printf("%*sIF_STMT:\n", indent, "");
            printf("%*sCondition:\n", indent + 2, "");
            print_ast(node->data.if_stmt.condition, indent + 4);
            printf("%*sIf branch:\n", indent + 2, "");
            print_ast(node->data.if_stmt.if_branch, indent + 4);
            if (node->data.if_stmt.else_branch) {
                printf("%*sElse branch:\n", indent + 2, "");
                print_ast(node->data.if_stmt.else_branch, indent + 4);
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

// test parsing simple if statement
void test_simple_if_statement() {
    printf("Testing simple if statement parsing...\n");
    
    const char *source = "if (1) print(42);";
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
    assert(ast->data.program.statements[0]->type == AST_IF_STMT);
    
    ASTNode *if_stmt = ast->data.program.statements[0];
    assert(if_stmt->data.if_stmt.condition != NULL);
    assert(if_stmt->data.if_stmt.condition->type == AST_NUMBER);
    assert(if_stmt->data.if_stmt.condition->data.number == 1.0);
    assert(if_stmt->data.if_stmt.if_branch != NULL);
    assert(if_stmt->data.if_stmt.if_branch->type == AST_PRINT_CALL);
    assert(if_stmt->data.if_stmt.else_branch == NULL);
    
    printf("Simple if statement AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("Simple if statement test passed!\n\n");
}

// test parsing if-else statement
void test_if_else_statement() {
    printf("Testing if-else statement parsing...\n");
    
    const char *source = "if (0) print(1) else print(2);";
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
    assert(ast->data.program.statements[0]->type == AST_IF_STMT);
    
    ASTNode *if_stmt = ast->data.program.statements[0];
    assert(if_stmt->data.if_stmt.condition != NULL);
    assert(if_stmt->data.if_stmt.condition->type == AST_NUMBER);
    assert(if_stmt->data.if_stmt.condition->data.number == 0.0);
    assert(if_stmt->data.if_stmt.if_branch != NULL);
    assert(if_stmt->data.if_stmt.if_branch->type == AST_PRINT_CALL);
    assert(if_stmt->data.if_stmt.else_branch != NULL);
    assert(if_stmt->data.if_stmt.else_branch->type == AST_PRINT_CALL);
    
    printf("If-else statement AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("If-else statement test passed!\n\n");
}

// test parsing if statement with variable condition
void test_if_with_variable_condition() {
    printf("Testing if statement with variable condition...\n");
    
    const char *source = "if (a > 5) print(1);";
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
    assert(ast->data.program.statements[0]->type == AST_IF_STMT);
    
    ASTNode *if_stmt = ast->data.program.statements[0];
    assert(if_stmt->data.if_stmt.condition != NULL);
    assert(if_stmt->data.if_stmt.condition->type == AST_BINARY_OP);
    assert(if_stmt->data.if_stmt.condition->data.binary.operator == '>');
    assert(if_stmt->data.if_stmt.if_branch != NULL);
    assert(if_stmt->data.if_stmt.if_branch->type == AST_PRINT_CALL);
    assert(if_stmt->data.if_stmt.else_branch == NULL);
    
    printf("If with variable condition AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("If with variable condition test passed!\n\n");
}

// test parsing if statement with different statement types
void test_if_with_different_statements() {
    printf("Testing if statement with different statement types...\n");
    
    // Test if with let declaration
    const char *source1 = "if (1) let x = 42;";
    Lexer *lexer = lexer_create(source1);
    Parser *parser = parser_create(lexer);
    
    ASTNode *ast = parser_parse(parser);
    
    if (parser_has_error(parser)) {
        printf("Parser error: %s\n", parser_get_error(parser));
        assert(0);
    }
    
    assert(ast != NULL);
    assert(ast->type == AST_PROGRAM);
    assert(ast->data.program.count == 1);
    assert(ast->data.program.statements[0]->type == AST_IF_STMT);
    
    ASTNode *if_stmt = ast->data.program.statements[0];
    assert(if_stmt->data.if_stmt.if_branch->type == AST_LET_DECL);
    
    printf("If with let declaration AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test if with expression statement
    const char *source2 = "if (0) x + 5 else y * 2;";
    lexer = lexer_create(source2);
    parser = parser_create(lexer);
    
    ast = parser_parse(parser);
    
    if (parser_has_error(parser)) {
        printf("Parser error: %s\n", parser_get_error(parser));
        assert(0);
    }
    
    assert(ast != NULL);
    assert(ast->type == AST_PROGRAM);
    assert(ast->data.program.count == 1);
    assert(ast->data.program.statements[0]->type == AST_IF_STMT);
    
    if_stmt = ast->data.program.statements[0];
    assert(if_stmt->data.if_stmt.if_branch->type == AST_BINARY_OP);
    assert(if_stmt->data.if_stmt.else_branch->type == AST_BINARY_OP);
    
    printf("If with expression statements AST:\n");
    print_ast(ast, 0);
    
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("If with different statement types test passed!\n\n");
}

// test if statement error handling
void test_if_statement_error_handling() {
    printf("Testing if statement error handling...\n");
    
    // Test missing opening parenthesis
    const char *source1 = "if 1) print(42);";
    Lexer *lexer = lexer_create(source1);
    Parser *parser = parser_create(lexer);
    
    ASTNode *ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("Missing opening parenthesis error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test missing closing parenthesis
    const char *source2 = "if (1 print(42);";
    lexer = lexer_create(source2);
    parser = parser_create(lexer);
    
    ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("Missing closing parenthesis error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test missing condition
    const char *source3 = "if () print(42);";
    lexer = lexer_create(source3);
    parser = parser_create(lexer);
    
    ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("Missing condition error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test missing if statement
    const char *source4 = "if (1);";
    lexer = lexer_create(source4);
    parser = parser_create(lexer);
    
    ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("Missing if statement error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test missing else statement
    const char *source5 = "if (1) print(1) else;";
    lexer = lexer_create(source5);
    parser = parser_create(lexer);
    
    ast = parser_parse(parser);
    
    assert(parser_has_error(parser));
    assert(ast == NULL);
    
    printf("Missing else statement error: %s\n", parser_get_error(parser));
    
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    printf("If statement error handling test passed!\n\n");
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
    test_simple_if_statement();
    test_if_else_statement();
    test_if_with_variable_condition();
    test_if_with_different_statements();
    test_if_statement_error_handling();
    
    printf("All parser tests passed!\n");
    return 0;
}