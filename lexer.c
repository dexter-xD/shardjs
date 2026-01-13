/*
 * lexer.c - tokenizes shardjs source code
 * 
 * breaks source text into tokens (numbers, identifiers, operators, etc).
 * tracks line/column info for error reporting.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/token.h"
#include "include/runtime.h"

// lexer state - tracks position in source
struct Lexer {
    const char *source;
    size_t position;
    size_t length;
    char current_char;
    int line;
    int column;
};

// move to next char, update line/col
static void lexer_advance(Lexer *lexer) {
    if (lexer->position < lexer->length) {
        if (lexer->current_char == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        
        lexer->position++;
        if (lexer->position < lexer->length) {
            lexer->current_char = lexer->source[lexer->position];
        } else {
            lexer->current_char = '\0';
        }
    }
}

// look ahead one char without moving
static char lexer_peek(Lexer *lexer) {
    size_t next_pos = lexer->position + 1;
    if (next_pos < lexer->length) {
        return lexer->source[next_pos];
    }
    return '\0';
}

// skip spaces, tabs, newlines
static void lexer_skip_whitespace(Lexer *lexer) {
    while (lexer->current_char != '\0' && isspace(lexer->current_char)) {
        lexer_advance(lexer);
    }
}

// create new lexer for source string
Lexer* lexer_create(const char *source) {
    if (!source) {
        return NULL;
    }
    
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer) {
        return NULL;
    }
    
    lexer->source = source;
    lexer->position = 0;
    lexer->length = strlen(source);
    lexer->line = 1;
    lexer->column = 1;
    
    // set first char
    if (lexer->length > 0) {
        lexer->current_char = lexer->source[0];
    } else {
        lexer->current_char = '\0';
    }
    
    return lexer;
}

void lexer_destroy(Lexer *lexer) {
    if (lexer) {
        free(lexer);
    }
}

// token creation helpers
static Token create_token(TokenType type, int line, int column) {
    Token token;
    token.type = type;
    token.number = 0.0;
    token.text = NULL;
    token.line = line;
    token.column = column;
    return token;
}

static Token create_number_token(double value, int line, int column) {
    Token token = create_token(TOKEN_NUMBER, line, column);
    token.number = value;
    return token;
}

static Token create_text_token(TokenType type, const char *text, int line, int column) {
    Token token = create_token(type, line, column);
    if (text) {
        size_t len = strlen(text);
        token.text = malloc(len + 1);
        if (token.text) {
            strcpy(token.text, text);
        } else {
            token.type = TOKEN_ERROR; // out of memory
        }
    }
    return token;
}

void token_free(Token *token) {
    if (token && token->text) {
        free(token->text);
        token->text = NULL;
    }
}

// read number (int or float)
static Token lexer_read_number(Lexer *lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    char buffer[64];
    int pos = 0;
    
    // read digits before decimal
    while (lexer->current_char != '\0' && isdigit(lexer->current_char) && pos < 63) {
        buffer[pos++] = lexer->current_char;
        lexer_advance(lexer);
    }
    
    // handle decimal point
    if (lexer->current_char == '.' && isdigit(lexer_peek(lexer))) {
        buffer[pos++] = lexer->current_char;
        lexer_advance(lexer);
        
        while (lexer->current_char != '\0' && isdigit(lexer->current_char) && pos < 63) {
            buffer[pos++] = lexer->current_char;
            lexer_advance(lexer);
        }
    }
    
    buffer[pos] = '\0';
    double value = atof(buffer);
    
    return create_number_token(value, start_line, start_column);
}

// read identifier or keyword
static Token lexer_read_identifier(Lexer *lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    char buffer[64];
    int pos = 0;
    
    // read alphanumeric + underscore
    while (lexer->current_char != '\0' && 
           (isalnum(lexer->current_char) || lexer->current_char == '_') && 
           pos < 63) {
        buffer[pos++] = lexer->current_char;
        lexer_advance(lexer);
    }
    
    buffer[pos] = '\0';
    
    // check for keywords
    if (strcmp(buffer, "let") == 0) {
        return create_token(TOKEN_LET, start_line, start_column);
    }
    
    return create_text_token(TOKEN_IDENTIFIER, buffer, start_line, start_column);
}

// main tokenizer - returns next token from source
Token lexer_next_token(Lexer *lexer) {
    if (!lexer) {
        return create_token(TOKEN_ERROR, 0, 0);
    }
    
    lexer_skip_whitespace(lexer);
    
    if (lexer->current_char == '\0') {
        return create_token(TOKEN_EOF, lexer->line, lexer->column);
    }
    
    int current_line = lexer->line;
    int current_column = lexer->column;
    
    // numbers
    if (isdigit(lexer->current_char)) {
        return lexer_read_number(lexer);
    }
    
    // identifiers and keywords
    if (isalpha(lexer->current_char) || lexer->current_char == '_') {
        return lexer_read_identifier(lexer);
    }
    
    // single char tokens and comparison operators
    char ch = lexer->current_char;
    lexer_advance(lexer);
    
    switch (ch) {
        case '+':
            return create_token(TOKEN_PLUS, current_line, current_column);
        case '-':
            return create_token(TOKEN_MINUS, current_line, current_column);
        case '*':
            return create_token(TOKEN_MULTIPLY, current_line, current_column);
        case '/':
            return create_token(TOKEN_DIVIDE, current_line, current_column);
        case '=':
            // check for == (equality comparison)
            if (lexer->current_char == '=') {
                lexer_advance(lexer);
                return create_token(TOKEN_EQUAL, current_line, current_column);
            }
            return create_token(TOKEN_ASSIGN, current_line, current_column);
        case '>':
            // check for >= (greater than or equal)
            if (lexer->current_char == '=') {
                lexer_advance(lexer);
                return create_token(TOKEN_GREATER_EQUAL, current_line, current_column);
            }
            return create_token(TOKEN_GREATER, current_line, current_column);
        case '<':
            // check for <= (less than or equal)
            if (lexer->current_char == '=') {
                lexer_advance(lexer);
                return create_token(TOKEN_LESS_EQUAL, current_line, current_column);
            }
            return create_token(TOKEN_LESS, current_line, current_column);
        case '!':
            // check for != (not equal)
            if (lexer->current_char == '=') {
                lexer_advance(lexer);
                return create_token(TOKEN_NOT_EQUAL, current_line, current_column);
            }
            // standalone ! is not supported, return error
            return create_token(TOKEN_ERROR, current_line, current_column);
        case '(':
            return create_token(TOKEN_LPAREN, current_line, current_column);
        case ')':
            return create_token(TOKEN_RPAREN, current_line, current_column);
        case ';':
            return create_token(TOKEN_SEMICOLON, current_line, current_column);
        default:
            return create_token(TOKEN_ERROR, current_line, current_column);
    }
}