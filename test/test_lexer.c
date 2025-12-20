/*
 * test_lexer.c - tests for the lexer/tokenizer
 * 
 * tests tokenization of numbers, identifiers, operators,
 * position tracking, and error handling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/token.h"
#include "../include/runtime.h"

// helper to print token type
const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_LET: return "LET";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// helper to free token text
void free_token(Token *token) {
    if (token->text) {
        free(token->text);
        token->text = NULL;
    }
}

/* Test basic number tokenization */
void test_numbers() {
    printf("Testing number tokenization...\n");
    
    Lexer *lexer = lexer_create("42 3.14 0 123.456");
    assert(lexer != NULL);
    
    Token token = lexer_next_token(lexer);
    assert(token.type == TOKEN_NUMBER);
    assert(token.number == 42.0);
    assert(token.line == 1);
    assert(token.column == 1);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_NUMBER);
    assert(token.number == 3.14);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_NUMBER);
    assert(token.number == 0.0);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_NUMBER);
    assert(token.number == 123.456);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_EOF);
    
    lexer_destroy(lexer);
    printf("Number tokenization tests passed\n");
}

// test identifier and keyword tokenization
void test_identifiers() {
    printf("Testing identifier and keyword tokenization...\n");
    
    Lexer *lexer = lexer_create("let x variable_name _underscore");
    assert(lexer != NULL);
    
    Token token = lexer_next_token(lexer);
    assert(token.type == TOKEN_LET);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_IDENTIFIER);
    assert(strcmp(token.text, "x") == 0);
    free_token(&token);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_IDENTIFIER);
    assert(strcmp(token.text, "variable_name") == 0);
    free_token(&token);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_IDENTIFIER);
    assert(strcmp(token.text, "_underscore") == 0);
    free_token(&token);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_EOF);
    
    lexer_destroy(lexer);
    printf("Identifier and keyword tests passed\n");
}

// test operator tokenization
void test_operators() {
    printf("Testing operator tokenization...\n");
    
    Lexer *lexer = lexer_create("+ - * / = ( ) ;");
    assert(lexer != NULL);
    
    Token token = lexer_next_token(lexer);
    assert(token.type == TOKEN_PLUS);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_MINUS);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_MULTIPLY);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_DIVIDE);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_ASSIGN);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_LPAREN);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_RPAREN);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_SEMICOLON);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_EOF);
    
    lexer_destroy(lexer);
    printf("Operator tokenization tests passed\n");
}

// test position tracking
void test_position_tracking() {
    printf("Testing position tracking...\n");
    
    Lexer *lexer = lexer_create("let x = 42;\nlet y = 3.14;");
    assert(lexer != NULL);
    
    Token token = lexer_next_token(lexer);
    assert(token.type == TOKEN_LET);
    assert(token.line == 1);
    assert(token.column == 1);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_IDENTIFIER);
    assert(token.line == 1);
    assert(token.column == 5);
    free_token(&token);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_ASSIGN);
    assert(token.line == 1);
    assert(token.column == 7);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_NUMBER);
    assert(token.line == 1);
    assert(token.column == 9);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_SEMICOLON);
    assert(token.line == 1);
    assert(token.column == 11);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_LET);
    assert(token.line == 2);
    assert(token.column == 1);
    
    lexer_destroy(lexer);
    printf("Position tracking tests passed\n");
}

// test error handling
void test_error_handling() {
    printf("Testing error handling...\n");
    
    Lexer *lexer = lexer_create("let x = 42 @ invalid");
    assert(lexer != NULL);
    
    Token token = lexer_next_token(lexer);
    assert(token.type == TOKEN_LET);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_IDENTIFIER);
    free_token(&token);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_ASSIGN);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_NUMBER);
    
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_ERROR);
    assert(token.line == 1);
    assert(token.column == 12);
    
    lexer_destroy(lexer);
    printf("Error handling tests passed\n");
}

// test complete expression
void test_complete_expression() {
    printf("Testing complete expression...\n");
    
    Lexer *lexer = lexer_create("let result = (10 + 5) * 2;");
    assert(lexer != NULL);
    
    TokenType expected_types[] = {
        TOKEN_LET, TOKEN_IDENTIFIER, TOKEN_ASSIGN, TOKEN_LPAREN,
        TOKEN_NUMBER, TOKEN_PLUS, TOKEN_NUMBER, TOKEN_RPAREN,
        TOKEN_MULTIPLY, TOKEN_NUMBER, TOKEN_SEMICOLON, TOKEN_EOF
    };
    
    int num_expected = sizeof(expected_types) / sizeof(expected_types[0]);
    
    for (int i = 0; i < num_expected; i++) {
        Token token = lexer_next_token(lexer);
        assert(token.type == expected_types[i]);
        if (token.text) {
            free_token(&token);
        }
    }
    
    lexer_destroy(lexer);
    printf("Complete expression tests passed\n");
}

int main() {
    printf("Running lexer tests...\n\n");
    
    test_numbers();
    test_identifiers();
    test_operators();
    test_position_tracking();
    test_error_handling();
    test_complete_expression();
    
    printf("\nAll lexer tests passed!\n");
    return 0;
}