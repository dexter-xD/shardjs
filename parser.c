/*
 * parser.c - recursive descent parser for shardjs
 * 
 * converts tokens into an abstract syntax tree.
 * handles expressions, statements, and basic error recovery.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/token.h"
#include "include/runtime.h"

// parser state - tracks current and next tokens
struct Parser {
    Lexer *lexer;
    Token current_token;
    Token lookahead_token;
    int has_error;
    char error_message[256];
};

// internal helpers
static void parser_advance(Parser *parser);
static int parser_match(Parser *parser, TokenType type);
static int parser_consume(Parser *parser, TokenType type, const char *error_msg);
static void parser_error(Parser *parser, const char *message);

// parsing functions
static ASTNode* parse_expression(Parser *parser);
static ASTNode* parse_term(Parser *parser);
static ASTNode* parse_factor(Parser *parser);
static ASTNode* parse_statement(Parser *parser);
static ASTNode* parse_let_declaration(Parser *parser);
static ASTNode* parse_print_call(Parser *parser);

// create parser with lexer
Parser* parser_create(Lexer *lexer) {
    if (!lexer) {
        return NULL;
    }
    
    Parser *parser = malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }
    
    parser->lexer = lexer;
    parser->has_error = 0;
    parser->error_message[0] = '\0';
    
    // prime the pump with first two tokens
    parser->current_token = lexer_next_token(lexer);
    parser->lookahead_token = lexer_next_token(lexer);
    
    return parser;
}

void parser_destroy(Parser *parser) {
    if (!parser) {
        return;
    }
    
    token_free(&parser->current_token);
    token_free(&parser->lookahead_token);
    
    free(parser);
}

// move to next token
static void parser_advance(Parser *parser) {
    if (!parser) {
        return;
    }
    
    token_free(&parser->current_token);
    
    // shift tokens forward
    parser->current_token = parser->lookahead_token;
    parser->lookahead_token = lexer_next_token(parser->lexer);
}

// check if current token is of given type
static int parser_match(Parser *parser, TokenType type) {
    if (!parser) {
        return 0;
    }
    
    return parser->current_token.type == type;
}

// consume expected token or error
static int parser_consume(Parser *parser, TokenType type, const char *error_msg) {
    if (!parser) {
        return 0;
    }
    
    if (parser_match(parser, type)) {
        parser_advance(parser);
        return 1;
    } else {
        parser_error(parser, error_msg);
        return 0;
    }
}

// set error state with location info
static void parser_error(Parser *parser, const char *message) {
    if (!parser || !message) {
        return;
    }
    
    parser->has_error = 1;
    snprintf(parser->error_message, sizeof(parser->error_message), 
             "Parse error at line %d, column %d: %s", 
             parser->current_token.line, 
             parser->current_token.column, 
             message);
}

int parser_has_error(Parser *parser) {
    return parser ? parser->has_error : 1;
}

const char* parser_get_error(Parser *parser) {
    if (!parser) {
        return "Invalid parser";
    }
    
    return parser->has_error ? parser->error_message : "No error";
}

// parse + and - (lowest precedence)
static ASTNode* parse_expression(Parser *parser) {
    if (!parser || parser->has_error) {
        return NULL;
    }
    
    ASTNode *left = parse_term(parser);
    if (!left || parser->has_error) {
        return left;
    }
    
    while (parser_match(parser, TOKEN_PLUS) || parser_match(parser, TOKEN_MINUS)) {
        char operator = (parser->current_token.type == TOKEN_PLUS) ? '+' : '-';
        parser_advance(parser);
        
        ASTNode *right = parse_term(parser);
        if (!right || parser->has_error) {
            ast_destroy(left);
            return NULL;
        }
        
        left = ast_create_binary_op(left, operator, right);
        if (!left) {
            parser_error(parser, "Failed to create binary operation node");
            return NULL;
        }
    }
    
    return left;
}

// parse * and / (higher precedence)
static ASTNode* parse_term(Parser *parser) {
    if (!parser || parser->has_error) {
        return NULL;
    }
    
    ASTNode *left = parse_factor(parser);
    if (!left || parser->has_error) {
        return left;
    }
    
    while (parser_match(parser, TOKEN_MULTIPLY) || parser_match(parser, TOKEN_DIVIDE)) {
        char operator = (parser->current_token.type == TOKEN_MULTIPLY) ? '*' : '/';
        parser_advance(parser);
        
        ASTNode *right = parse_factor(parser);
        if (!right || parser->has_error) {
            ast_destroy(left);
            return NULL;
        }
        
        left = ast_create_binary_op(left, operator, right);
        if (!left) {
            parser_error(parser, "Failed to create binary operation node");
            return NULL;
        }
    }
    
    return left;
}

// parse numbers, identifiers, and (expr)
static ASTNode* parse_factor(Parser *parser) {
    if (!parser || parser->has_error) {
        return NULL;
    }
    
    if (parser_match(parser, TOKEN_NUMBER)) {
        double value = parser->current_token.number;
        parser_advance(parser);
        return ast_create_number(value);
    }
    
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        char *name = strdup(parser->current_token.text);
        if (!name) {
            parser_error(parser, "Memory allocation failed for identifier");
            return NULL;
        }
        parser_advance(parser);
        ASTNode *node = ast_create_identifier(name);
        free(name); // ast_create_identifier makes its own copy
        return node;
    }
    
    if (parser_match(parser, TOKEN_LPAREN)) {
        parser_advance(parser); // consume '('
        
        ASTNode *expr = parse_expression(parser);
        if (!expr || parser->has_error) {
            return NULL;
        }
        
        if (!parser_consume(parser, TOKEN_RPAREN, "Expected ')' after expression")) {
            ast_destroy(expr);
            return NULL;
        }
        
        return expr;
    }
    
    parser_error(parser, "Expected number, identifier, or '('");
    return NULL;
}

// main entry point - parse entire program
ASTNode* parser_parse(Parser *parser) {
    if (!parser) {
        return NULL;
    }
    
    // create program node to hold statements
    ASTNode *program = ast_create_program();
    if (!program) {
        parser_error(parser, "Failed to create program node");
        return NULL;
    }
    
    // parse statements until eof
    while (!parser_match(parser, TOKEN_EOF) && !parser->has_error) {
        ASTNode *stmt = parse_statement(parser);
        if (!stmt) {
            if (!parser->has_error) {
                parser_error(parser, "Failed to parse statement");
            }
            ast_destroy(program);
            return NULL;
        }
        
        if (!ast_program_add_statement(program, stmt)) {
            parser_error(parser, "Failed to add statement to program");
            ast_destroy(stmt);
            ast_destroy(program);
            return NULL;
        }
    }
    
    return program;
}

// parse let declarations or expressions
static ASTNode* parse_statement(Parser *parser) {
    if (!parser || parser->has_error) {
        return NULL;
    }
    
    if (parser_match(parser, TOKEN_LET)) {
        return parse_let_declaration(parser);
    }
    
    // check for print function call
    if (parser_match(parser, TOKEN_IDENTIFIER) && 
        parser->current_token.text && 
        strcmp(parser->current_token.text, "print") == 0) {
        return parse_print_call(parser);
    }
    
    // otherwise it's an expression
    ASTNode *expr = parse_expression(parser);
    if (!expr || parser->has_error) {
        return NULL;
    }
    
    // optional semicolon
    if (parser_match(parser, TOKEN_SEMICOLON)) {
        parser_advance(parser);
    }
    
    return expr;
}

// parse: let identifier = expression;
static ASTNode* parse_let_declaration(Parser *parser) {
    if (!parser || parser->has_error) {
        return NULL;
    }
    
    // consume 'let'
    if (!parser_consume(parser, TOKEN_LET, "Expected 'let' keyword")) {
        return NULL;
    }
    
    // get variable name
    if (!parser_match(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected identifier after 'let'");
        return NULL;
    }
    
    char *name = strdup(parser->current_token.text);
    if (!name) {
        parser_error(parser, "Memory allocation failed for variable name");
        return NULL;
    }
    parser_advance(parser);
    
    // consume '='
    if (!parser_consume(parser, TOKEN_ASSIGN, "Expected '=' after variable name")) {
        free(name);
        return NULL;
    }
    
    // parse value
    ASTNode *value = parse_expression(parser);
    if (!value || parser->has_error) {
        free(name);
        return NULL;
    }
    
    // consume ';'
    if (!parser_consume(parser, TOKEN_SEMICOLON, "Expected ';' after let declaration")) {
        free(name);
        ast_destroy(value);
        return NULL;
    }
    
    ASTNode *let_node = ast_create_let_decl(name, value);
    free(name); // ast_create_let_decl makes its own copy
    
    if (!let_node) {
        parser_error(parser, "Failed to create let declaration node");
        ast_destroy(value);
        return NULL;
    }
    
    return let_node;
}

// parse: print(expression);
static ASTNode* parse_print_call(Parser *parser) {
    if (!parser || parser->has_error) {
        return NULL;
    }
    
    // consume 'print'
    if (!parser_consume(parser, TOKEN_IDENTIFIER, "Expected 'print'")) {
        return NULL;
    }
    
    // consume '('
    if (!parser_consume(parser, TOKEN_LPAREN, "Expected '(' after 'print'")) {
        return NULL;
    }
    
    // parse argument
    ASTNode *arg = parse_expression(parser);
    if (!arg || parser->has_error) {
        return NULL;
    }
    
    // consume ')'
    if (!parser_consume(parser, TOKEN_RPAREN, "Expected ')' after print argument")) {
        ast_destroy(arg);
        return NULL;
    }
    
    // optional semicolon
    if (parser_match(parser, TOKEN_SEMICOLON)) {
        parser_advance(parser);
    }
    
    ASTNode *print_node = ast_create_print_call(arg);
    if (!print_node) {
        parser_error(parser, "Failed to create print call node");
        ast_destroy(arg);
        return NULL;
    }
    
    return print_node;
}