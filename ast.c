/*
 * ast.c - abstract syntax tree nodes for shardjs
 * 
 * creates and manages ast nodes for different language constructs.
 * handles memory allocation and cleanup for the parse tree.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/runtime.h"

ASTNode* ast_create_number(double value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_NUMBER;
    node->data.number = value;
    return node;
}

ASTNode* ast_create_identifier(const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_IDENTIFIER;
    node->data.identifier = strdup(name);
    if (!node->data.identifier) {
        free(node);
        return NULL;
    }
    return node;
}

ASTNode* ast_create_binary_op(ASTNode *left, char operator, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_BINARY_OP;
    node->data.binary.left = left;
    node->data.binary.right = right;
    node->data.binary.operator = operator;
    return node;
}

ASTNode* ast_create_let_decl(const char *name, ASTNode *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_LET_DECL;
    node->data.let_decl.name = strdup(name);
    node->data.let_decl.value = value;
    if (!node->data.let_decl.name) {
        free(node);
        return NULL;
    }
    return node;
}

ASTNode* ast_create_print_call(ASTNode *arg) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_PRINT_CALL;
    node->data.print_arg = arg;
    return node;
}

ASTNode* ast_create_program(void) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = AST_PROGRAM;
    node->data.program.statements = NULL;
    node->data.program.count = 0;
    node->data.program.capacity = 0;
    return node;
}

// add statement to program node - grows array as needed
int ast_program_add_statement(ASTNode *program, ASTNode *statement) {
    if (!program || program->type != AST_PROGRAM || !statement) {
        return 0;
    }
    
    // grow array if we're out of space
    if (program->data.program.count >= program->data.program.capacity) {
        int new_capacity = program->data.program.capacity == 0 ? 4 : program->data.program.capacity * 2;
        ASTNode **new_statements = realloc(program->data.program.statements, 
                                          new_capacity * sizeof(ASTNode*));
        if (!new_statements) {
            return 0;
        }
        program->data.program.statements = new_statements;
        program->data.program.capacity = new_capacity;
    }
    
    program->data.program.statements[program->data.program.count] = statement;
    program->data.program.count++;
    return 1;
}

// recursively free ast node and all children
void ast_destroy(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_IDENTIFIER:
            free(node->data.identifier);
            break;
        case AST_BINARY_OP:
            ast_destroy(node->data.binary.left);
            ast_destroy(node->data.binary.right);
            break;
        case AST_LET_DECL:
            free(node->data.let_decl.name);
            ast_destroy(node->data.let_decl.value);
            break;
        case AST_PRINT_CALL:
            ast_destroy(node->data.print_arg);
            break;
        case AST_PROGRAM:
            for (int i = 0; i < node->data.program.count; i++) {
                ast_destroy(node->data.program.statements[i]);
            }
            free(node->data.program.statements);
            break;
        case AST_NUMBER:
            // numbers don't need cleanup
            break;
    }
    
    free(node);
}