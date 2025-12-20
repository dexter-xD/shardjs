/*
 * interpreter.c - executes shardjs ast nodes
 * 
 * walks the parse tree and evaluates expressions, handles variables,
 * and executes statements. includes basic error handling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "include/runtime.h"

// simple error tracking
static int interpreter_error = 0;
static char interpreter_error_msg[256];

static void set_interpreter_error(const char *message) {
    interpreter_error = 1;
    snprintf(interpreter_error_msg, sizeof(interpreter_error_msg), "%s", message);
}

int interpreter_has_error(void) {
    return interpreter_error;
}

const char* interpreter_get_error(void) {
    return interpreter_error_msg;
}

void interpreter_clear_error(void) {
    interpreter_error = 0;
    interpreter_error_msg[0] = '\0';
}

// main eval function - walks ast and executes nodes
double interpret(ASTNode *node, Environment *env) {
    if (!node) {
        set_interpreter_error("Null AST node");
        return 0.0;
    }
    
    if (!env) {
        set_interpreter_error("Null environment");
        return 0.0;
    }
    
    interpreter_clear_error();
    
    switch (node->type) {
        case AST_NUMBER:
            return node->data.number;
            
        case AST_IDENTIFIER: {
            double value;
            if (env_get(env, node->data.identifier, &value)) {
                return value;
            } else {
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg), "Undefined variable: %s", node->data.identifier);
                set_interpreter_error(error_msg);
                return 0.0;
            }
        }
        
        case AST_BINARY_OP: {
            // eval both sides first
            double left_val = interpret(node->data.binary.left, env);
            if (interpreter_has_error()) {
                return 0.0;
            }
            
            double right_val = interpret(node->data.binary.right, env);
            if (interpreter_has_error()) {
                return 0.0;
            }
            
            // do the math
            switch (node->data.binary.operator) {
                case '+':
                    return left_val + right_val;
                case '-':
                    return left_val - right_val;
                case '*':
                    return left_val * right_val;
                case '/':
                    if (right_val == 0.0) {
                        set_interpreter_error("Division by zero");
                        return 0.0;
                    }
                    return left_val / right_val;
                default:
                    {
                        char error_msg[256];
                        snprintf(error_msg, sizeof(error_msg), "Unknown binary operator: %c", node->data.binary.operator);
                        set_interpreter_error(error_msg);
                        return 0.0;
                    }
            }
        }
        
        case AST_LET_DECL: {
            // eval the value and store it
            double value = interpret(node->data.let_decl.value, env);
            if (interpreter_has_error()) {
                return 0.0;
            }
            
            if (!env_set(env, node->data.let_decl.name, value)) {
                set_interpreter_error("Failed to store variable in environment");
                return 0.0;
            }
            
            return value;
        }
        
        case AST_PRINT_CALL: {
            // eval arg and print it
            double value = interpret(node->data.print_arg, env);
            if (interpreter_has_error()) {
                return 0.0;
            }
            
            printf("%.15g\n", value);
            return value;
        }
        
        case AST_PROGRAM: {
            // run all statements, return last result
            double last_result = 0.0;
            
            for (int i = 0; i < node->data.program.count; i++) {
                last_result = interpret(node->data.program.statements[i], env);
                if (interpreter_has_error()) {
                    return 0.0;
                }
            }
            
            return last_result;
        }
        
        default:
            set_interpreter_error("Unsupported AST node type in interpreter core");
            return 0.0;
    }
}