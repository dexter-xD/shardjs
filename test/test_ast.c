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

void test_ast_create_if_stmt() {
    printf("Testing ast_create_if_stmt...\n");
    
    // create if statement: if (x > 5) print(1) else print(0)
    ASTNode *condition = ast_create_binary_op(
        ast_create_identifier("x"), 
        '>', 
        ast_create_number(5.0)
    );
    ASTNode *if_branch = ast_create_print_call(ast_create_number(1.0));
    ASTNode *else_branch = ast_create_print_call(ast_create_number(0.0));
    
    ASTNode *if_stmt = ast_create_if_stmt(condition, if_branch, else_branch);
    
    assert(if_stmt != NULL);
    assert(if_stmt->type == AST_IF_STMT);
    assert(if_stmt->data.if_stmt.condition == condition);
    assert(if_stmt->data.if_stmt.if_branch == if_branch);
    assert(if_stmt->data.if_stmt.else_branch == else_branch);
    
    ast_destroy(if_stmt);  // should recursively destroy all branches
    printf("ast_create_if_stmt passed\n");
}

void test_ast_create_if_stmt_no_else() {
    printf("Testing ast_create_if_stmt without else branch...\n");
    
    // create if statement without else: if (1) print(42)
    ASTNode *condition = ast_create_number(1.0);
    ASTNode *if_branch = ast_create_print_call(ast_create_number(42.0));
    
    ASTNode *if_stmt = ast_create_if_stmt(condition, if_branch, NULL);
    
    assert(if_stmt != NULL);
    assert(if_stmt->type == AST_IF_STMT);
    assert(if_stmt->data.if_stmt.condition == condition);
    assert(if_stmt->data.if_stmt.if_branch == if_branch);
    assert(if_stmt->data.if_stmt.else_branch == NULL);
    
    ast_destroy(if_stmt);  // should handle NULL else_branch gracefully
    printf("ast_create_if_stmt without else passed\n");
}

void test_ast_if_stmt_memory_management() {
    printf("Testing if statement memory management...\n");
    
    // create complex nested if statement
    ASTNode *var_a = ast_create_identifier("a");
    ASTNode *var_b = ast_create_identifier("b");
    ASTNode *condition = ast_create_binary_op(var_a, '+', var_b);
    
    ASTNode *if_branch = ast_create_let_decl("result", ast_create_number(1.0));
    ASTNode *else_branch = ast_create_let_decl("result", ast_create_number(0.0));
    
    ASTNode *if_stmt = ast_create_if_stmt(condition, if_branch, else_branch);
    
    // verify structure
    assert(if_stmt->type == AST_IF_STMT);
    assert(if_stmt->data.if_stmt.condition->type == AST_BINARY_OP);
    assert(if_stmt->data.if_stmt.if_branch->type == AST_LET_DECL);
    assert(if_stmt->data.if_stmt.else_branch->type == AST_LET_DECL);
    
    // this should recursively free all nodes
    ast_destroy(if_stmt);
    
    printf("Complex if statement memory management passed\n");
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
    test_ast_create_if_stmt();
    test_ast_create_if_stmt_no_else();
    test_ast_if_stmt_memory_management();
    
    printf("\nAll AST tests passed!\n");
    return 0;
}