/*
 * test_ast.c - tests for ast node creation and management
 * 
 * verifies ast node creation, memory management, and proper
 * handling of different node types.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/runtime.h"

void test_ast_create_number() {
    printf("Testing ast_create_number...\n");
    
    ASTNode *node = ast_create_number(42.5);
    assert(node != NULL);
    assert(node->type == AST_NUMBER);
    assert(node->data.number == 42.5);
    
    ast_destroy(node);
    printf("ast_create_number passed\n");
}

void test_ast_create_identifier() {
    printf("Testing ast_create_identifier...\n");
    
    ASTNode *node = ast_create_identifier("myVar");
    assert(node != NULL);
    assert(node->type == AST_IDENTIFIER);
    assert(strcmp(node->data.identifier, "myVar") == 0);
    
    ast_destroy(node);
    printf("ast_create_identifier passed\n");
}

void test_ast_create_binary_op() {
    printf("Testing ast_create_binary_op...\n");
    
    ASTNode *left = ast_create_number(10.0);
    ASTNode *right = ast_create_number(20.0);
    ASTNode *node = ast_create_binary_op(left, '+', right);
    
    assert(node != NULL);
    assert(node->type == AST_BINARY_OP);
    assert(node->data.binary.operator == '+');
    assert(node->data.binary.left == left);
    assert(node->data.binary.right == right);
    
    ast_destroy(node);  // should recursively destroy left and right
    printf("ast_create_binary_op passed\n");
}

void test_ast_create_let_decl() {
    printf("Testing ast_create_let_decl...\n");
    
    ASTNode *value = ast_create_number(100.0);
    ASTNode *node = ast_create_let_decl("testVar", value);
    
    assert(node != NULL);
    assert(node->type == AST_LET_DECL);
    assert(strcmp(node->data.let_decl.name, "testVar") == 0);
    assert(node->data.let_decl.value == value);
    
    ast_destroy(node);  // should recursively destroy value
    printf("ast_create_let_decl passed\n");
}

void test_ast_create_print_call() {
    printf("Testing ast_create_print_call...\n");
    
    ASTNode *arg = ast_create_identifier("x");
    ASTNode *node = ast_create_print_call(arg);
    
    assert(node != NULL);
    assert(node->type == AST_PRINT_CALL);
    assert(node->data.print_arg == arg);
    
    ast_destroy(node);  // should recursively destroy arg
    printf("ast_create_print_call passed\n");
}

void test_ast_memory_management() {
    printf("Testing AST memory management...\n");
    
    // create complex ast: let x = (10 + 20) * 5;
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *twenty = ast_create_number(20.0);
    ASTNode *five = ast_create_number(5.0);
    
    ASTNode *add = ast_create_binary_op(ten, '+', twenty);
    ASTNode *mult = ast_create_binary_op(add, '*', five);
    ASTNode *let_decl = ast_create_let_decl("x", mult);
    
    // verify structure
    assert(let_decl->type == AST_LET_DECL);
    assert(strcmp(let_decl->data.let_decl.name, "x") == 0);
    assert(let_decl->data.let_decl.value->type == AST_BINARY_OP);
    
    // this should recursively free all nodes
    ast_destroy(let_decl);
    
    printf("Complex AST memory management passed\n");
}

void test_ast_null_handling() {
    printf("Testing AST null handling...\n");
    
    // ast_destroy should handle null gracefully
    ast_destroy(NULL);
    
    printf("NULL handling passed\n");
}

int main() {
    printf("Running AST tests...\n\n");
    
    test_ast_create_number();
    test_ast_create_identifier();
    test_ast_create_binary_op();
    test_ast_create_let_decl();
    test_ast_create_print_call();
    test_ast_memory_management();
    test_ast_null_handling();
    
    printf("\nAll AST tests passed!\n");
    return 0;
}