/*
 * runtime.h - main header for shardjs interpreter
 * 
 * defines ast nodes, opaque types, and function interfaces
 * for lexer, parser, ast, environment, and interpreter.
 */

#ifndef RUNTIME_H
#define RUNTIME_H

#include "token.h"

// ast node types
typedef enum {
    AST_NUMBER,
    AST_IDENTIFIER,
    AST_BINARY_OP,
    AST_LET_DECL,
    AST_PRINT_CALL,
    AST_PROGRAM,
    AST_IF_STMT
} ASTNodeType;

// ast node structure
typedef struct ASTNode {
    ASTNodeType type;
    union {
        double number;
        char *identifier;
        struct {
            struct ASTNode *left;
            struct ASTNode *right;
            char operator;
        } binary;
        struct {
            char *name;
            struct ASTNode *value;
        } let_decl;
        struct ASTNode *print_arg;
        struct {
            struct ASTNode **statements;
            int count;
            int capacity;
        } program;
        struct {
            struct ASTNode *condition;
            struct ASTNode *if_branch;
            struct ASTNode *else_branch;  // NULL if no else
        } if_stmt;
    } data;
} ASTNode;

// opaque types
typedef struct Lexer Lexer;
typedef struct Parser Parser;
typedef struct Environment Environment;

// lexer interface
Lexer* lexer_create(const char *source);
void lexer_destroy(Lexer *lexer);
Token lexer_next_token(Lexer *lexer);

// parser interface
Parser* parser_create(Lexer *lexer);
void parser_destroy(Parser *parser);
ASTNode* parser_parse(Parser *parser);
int parser_has_error(Parser *parser);
const char* parser_get_error(Parser *parser);

// ast interface
ASTNode* ast_create_number(double value);
ASTNode* ast_create_identifier(const char *name);
ASTNode* ast_create_binary_op(ASTNode *left, char operator, ASTNode *right);
ASTNode* ast_create_let_decl(const char *name, ASTNode *value);
ASTNode* ast_create_print_call(ASTNode *arg);
ASTNode* ast_create_program(void);
ASTNode* ast_create_if_stmt(ASTNode *condition, ASTNode *if_branch, ASTNode *else_branch);
int ast_program_add_statement(ASTNode *program, ASTNode *statement);
void ast_destroy(ASTNode *node);

// environment interface
Environment* env_create(void);
void env_destroy(Environment *env);
int env_set(Environment *env, const char *name, double value);
int env_get(Environment *env, const char *name, double *value);

// interpreter interface
double interpret(ASTNode *node, Environment *env);
int interpreter_has_error(void);
const char* interpreter_get_error(void);
void interpreter_clear_error(void);

#endif