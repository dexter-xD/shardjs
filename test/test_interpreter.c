/*
 * test_interpreter.c - tests for the interpreter core
 * 
 * tests expression evaluation, variable handling, print functions,
 * and comprehensive error handling scenarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../include/runtime.h"

void test_interpret_number() {
    printf("Testing number literal interpretation...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    ASTNode *node = ast_create_number(42.5);
    assert(node != NULL);
    
    double result = interpret(node, env);
    assert(!interpreter_has_error());
    assert(result == 42.5);
    
    ast_destroy(node);
    env_destroy(env);
    printf("Number literal test passed\n");
}

void test_interpret_identifier_success() {
    printf("Testing identifier lookup (success)...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // set a variable in the environment
    assert(env_set(env, "x", 123.45));
    
    ASTNode *node = ast_create_identifier("x");
    assert(node != NULL);
    
    double result = interpret(node, env);
    assert(!interpreter_has_error());
    assert(result == 123.45);
    
    ast_destroy(node);
    env_destroy(env);
    printf("Identifier lookup success test passed\n");
}

void test_interpret_identifier_undefined() {
    printf("Testing identifier lookup (undefined variable)...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    ASTNode *node = ast_create_identifier("undefined_var");
    assert(node != NULL);
    
    interpret(node, env);  // we don't need the result, just check for error
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Undefined variable: undefined_var") != NULL);
    
    ast_destroy(node);
    env_destroy(env);
    printf("Undefined variable test passed\n");
}

void test_interpret_null_node() {
    printf("Testing null node error handling...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    interpret(NULL, env);  // we don't need the result, just check for error
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Null AST node") != NULL);
    
    env_destroy(env);
    printf("Null node test passed\n");
}

void test_interpret_null_env() {
    printf("Testing null environment error handling...\n");
    
    ASTNode *node = ast_create_number(42.0);
    assert(node != NULL);
    
    interpret(node, NULL);  // we don't need the result, just check for error
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Null environment") != NULL);
    
    ast_destroy(node);
    printf("Null environment test passed\n");
}

void test_interpret_addition() {
    printf("Testing addition operation...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    ASTNode *left = ast_create_number(10.5);
    ASTNode *right = ast_create_number(5.3);
    ASTNode *add_node = ast_create_binary_op(left, '+', right);
    assert(add_node != NULL);
    
    double result = interpret(add_node, env);
    assert(!interpreter_has_error());
    assert(result == 15.8);
    
    ast_destroy(add_node);
    env_destroy(env);
    printf("Addition test passed\n");
}

void test_interpret_subtraction() {
    printf("Testing subtraction operation...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    ASTNode *left = ast_create_number(10.0);
    ASTNode *right = ast_create_number(3.0);
    ASTNode *sub_node = ast_create_binary_op(left, '-', right);
    assert(sub_node != NULL);
    
    double result = interpret(sub_node, env);
    assert(!interpreter_has_error());
    assert(result == 7.0);
    
    ast_destroy(sub_node);
    env_destroy(env);
    printf("Subtraction test passed\n");
}

void test_interpret_multiplication() {
    printf("Testing multiplication operation...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    ASTNode *left = ast_create_number(4.0);
    ASTNode *right = ast_create_number(2.5);
    ASTNode *mul_node = ast_create_binary_op(left, '*', right);
    assert(mul_node != NULL);
    
    double result = interpret(mul_node, env);
    assert(!interpreter_has_error());
    assert(result == 10.0);
    
    ast_destroy(mul_node);
    env_destroy(env);
    printf("Multiplication test passed\n");
}

void test_interpret_division() {
    printf("Testing division operation...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    ASTNode *left = ast_create_number(15.0);
    ASTNode *right = ast_create_number(3.0);
    ASTNode *div_node = ast_create_binary_op(left, '/', right);
    assert(div_node != NULL);
    
    double result = interpret(div_node, env);
    assert(!interpreter_has_error());
    assert(result == 5.0);
    
    ast_destroy(div_node);
    env_destroy(env);
    printf("Division test passed\n");
}

void test_interpret_division_by_zero() {
    printf("Testing division by zero error...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    ASTNode *left = ast_create_number(10.0);
    ASTNode *right = ast_create_number(0.0);
    ASTNode *div_node = ast_create_binary_op(left, '/', right);
    assert(div_node != NULL);
    
    interpret(div_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Division by zero") != NULL);
    
    ast_destroy(div_node);
    env_destroy(env);
    printf("Division by zero test passed\n");
}

void test_interpret_complex_expression() {
    printf("Testing complex expression with precedence...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // create expression: (2 + 3) * 4 = 20
    ASTNode *two = ast_create_number(2.0);
    ASTNode *three = ast_create_number(3.0);
    ASTNode *four = ast_create_number(4.0);
    ASTNode *add_node = ast_create_binary_op(two, '+', three);
    ASTNode *mul_node = ast_create_binary_op(add_node, '*', four);
    
    double result = interpret(mul_node, env);
    assert(!interpreter_has_error());
    assert(result == 20.0);
    
    ast_destroy(mul_node);
    env_destroy(env);
    printf("Complex expression test passed\n");
}

void test_interpret_expression_with_variables() {
    printf("Testing expression with variables...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // set variables x = 5, y = 3
    assert(env_set(env, "x", 5.0));
    assert(env_set(env, "y", 3.0));
    
    // create expression: x * y + 2 = 17
    ASTNode *x_node = ast_create_identifier("x");
    ASTNode *y_node = ast_create_identifier("y");
    ASTNode *two = ast_create_number(2.0);
    ASTNode *mul_node = ast_create_binary_op(x_node, '*', y_node);
    ASTNode *add_node = ast_create_binary_op(mul_node, '+', two);
    
    double result = interpret(add_node, env);
    assert(!interpreter_has_error());
    assert(result == 17.0);
    
    ast_destroy(add_node);
    env_destroy(env);
    printf("Expression with variables test passed\n");
}

void test_interpret_let_declaration() {
    printf("Testing let variable declaration...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // create let x = 42.5;
    ASTNode *value_node = ast_create_number(42.5);
    ASTNode *let_node = ast_create_let_decl("x", value_node);
    assert(let_node != NULL);
    
    double result = interpret(let_node, env);
    assert(!interpreter_has_error());
    assert(result == 42.5);  // let declaration returns the assigned value
    
    // verify the variable was stored in the environment
    double stored_value;
    assert(env_get(env, "x", &stored_value));
    assert(stored_value == 42.5);
    
    ast_destroy(let_node);
    env_destroy(env);
    printf("Let declaration test passed\n");
}

void test_interpret_let_with_expression() {
    printf("Testing let declaration with expression...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // create let y = 10 + 5;
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *five = ast_create_number(5.0);
    ASTNode *add_node = ast_create_binary_op(ten, '+', five);
    ASTNode *let_node = ast_create_let_decl("y", add_node);
    assert(let_node != NULL);
    
    double result = interpret(let_node, env);
    assert(!interpreter_has_error());
    assert(result == 15.0);
    
    // verify the variable was stored correctly
    double stored_value;
    assert(env_get(env, "y", &stored_value));
    assert(stored_value == 15.0);
    
    ast_destroy(let_node);
    env_destroy(env);
    printf("Let declaration with expression test passed\n");
}

void test_interpret_let_redeclaration() {
    printf("Testing let variable redeclaration...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // first declaration: let z = 100;
    ASTNode *value1 = ast_create_number(100.0);
    ASTNode *let1 = ast_create_let_decl("z", value1);
    
    double result1 = interpret(let1, env);
    assert(!interpreter_has_error());
    assert(result1 == 100.0);
    
    // second declaration: let z = 200; (redeclaration)
    ASTNode *value2 = ast_create_number(200.0);
    ASTNode *let2 = ast_create_let_decl("z", value2);
    
    double result2 = interpret(let2, env);
    assert(!interpreter_has_error());
    assert(result2 == 200.0);
    
    // verify the variable was updated
    double stored_value;
    assert(env_get(env, "z", &stored_value));
    assert(stored_value == 200.0);
    
    ast_destroy(let1);
    ast_destroy(let2);
    env_destroy(env);
    printf("Let redeclaration test passed\n");
}

void test_interpret_let_with_variable_reference() {
    printf("Testing let declaration with variable reference...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    /* First set a variable: let a = 10; */
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *let_a = ast_create_let_decl("a", ten);
    
    double result1 = interpret(let_a, env);
    assert(!interpreter_has_error());
    assert(result1 == 10.0);
    
    /* Then declare: let b = a * 2; */
    ASTNode *a_ref = ast_create_identifier("a");
    ASTNode *two = ast_create_number(2.0);
    ASTNode *mul_node = ast_create_binary_op(a_ref, '*', two);
    ASTNode *let_b = ast_create_let_decl("b", mul_node);
    
    double result2 = interpret(let_b, env);
    assert(!interpreter_has_error());
    assert(result2 == 20.0);
    
    /* Verify both variables are stored correctly */
    double a_value, b_value;
    assert(env_get(env, "a", &a_value));
    assert(env_get(env, "b", &b_value));
    assert(a_value == 10.0);
    assert(b_value == 20.0);
    
    ast_destroy(let_a);
    ast_destroy(let_b);
    env_destroy(env);
    printf("Let declaration with variable reference test passed\n");
}

void test_interpret_print_number() {
    printf("Testing print function with number...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // create print(42.5);
    ASTNode *value_node = ast_create_number(42.5);
    ASTNode *print_node = ast_create_print_call(value_node);
    assert(print_node != NULL);
    
    printf("Expected output: 42.5\nActual output: ");
    double result = interpret(print_node, env);
    assert(!interpreter_has_error());
    assert(result == 42.5);  // print returns the printed value
    
    ast_destroy(print_node);
    env_destroy(env);
    printf("Print number test passed\n");
}

void test_interpret_print_expression() {
    printf("Testing print function with expression...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // create print(10 + 5);
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *five = ast_create_number(5.0);
    ASTNode *add_node = ast_create_binary_op(ten, '+', five);
    ASTNode *print_node = ast_create_print_call(add_node);
    assert(print_node != NULL);
    
    printf("Expected output: 15\nActual output: ");
    double result = interpret(print_node, env);
    assert(!interpreter_has_error());
    assert(result == 15.0);
    
    ast_destroy(print_node);
    env_destroy(env);
    printf("Print expression test passed\n");
}

void test_interpret_print_variable() {
    printf("Testing print function with variable...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // set variable x = 123.456
    assert(env_set(env, "x", 123.456));
    
    // create print(x);
    ASTNode *x_node = ast_create_identifier("x");
    ASTNode *print_node = ast_create_print_call(x_node);
    assert(print_node != NULL);
    
    printf("Expected output: 123.456\nActual output: ");
    double result = interpret(print_node, env);
    assert(!interpreter_has_error());
    assert(result == 123.456);
    
    ast_destroy(print_node);
    env_destroy(env);
    printf("Print variable test passed\n");
}

void test_interpret_print_undefined_variable() {
    printf("Testing print function with undefined variable...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // create print(undefined_var);
    ASTNode *undefined_node = ast_create_identifier("undefined_var");
    ASTNode *print_node = ast_create_print_call(undefined_node);
    assert(print_node != NULL);
    
    interpret(print_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Undefined variable: undefined_var") != NULL);
    
    ast_destroy(print_node);
    env_destroy(env);
    printf("Print undefined variable test passed\n");
}

void test_interpreter_error_propagation() {
    printf("Testing error propagation in complex expressions...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // create expression with undefined variable: (5 + undefined_var) * 2
    ASTNode *five = ast_create_number(5.0);
    ASTNode *undefined_node = ast_create_identifier("undefined_var");
    ASTNode *two = ast_create_number(2.0);
    ASTNode *add_node = ast_create_binary_op(five, '+', undefined_node);
    ASTNode *mul_node = ast_create_binary_op(add_node, '*', two);
    
    interpret(mul_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Undefined variable: undefined_var") != NULL);
    
    ast_destroy(mul_node);
    env_destroy(env);
    printf("Error propagation test passed\n");
}

void test_interpreter_division_by_zero_in_expression() {
    printf("Testing division by zero in complex expression...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // create expression: 10 + (5 / 0)
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *five = ast_create_number(5.0);
    ASTNode *zero = ast_create_number(0.0);
    ASTNode *div_node = ast_create_binary_op(five, '/', zero);
    ASTNode *add_node = ast_create_binary_op(ten, '+', div_node);
    
    interpret(add_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Division by zero") != NULL);
    
    ast_destroy(add_node);
    env_destroy(env);
    printf("Division by zero in expression test passed\n");
}

void test_interpreter_let_with_error() {
    printf("Testing let declaration with error in value expression...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // create let x = 10 / 0;
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *zero = ast_create_number(0.0);
    ASTNode *div_node = ast_create_binary_op(ten, '/', zero);
    ASTNode *let_node = ast_create_let_decl("x", div_node);
    
    interpret(let_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Division by zero") != NULL);
    
    // verify the variable was not stored due to error
    double value;
    assert(!env_get(env, "x", &value));  // should not exist
    
    ast_destroy(let_node);
    env_destroy(env);
    printf("Let declaration with error test passed\n");
}

void test_interpreter_error_recovery() {
    printf("Testing interpreter error recovery...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // first operation with error
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *zero = ast_create_number(0.0);
    ASTNode *div_node = ast_create_binary_op(ten, '/', zero);
    
    interpret(div_node, env);
    assert(interpreter_has_error());
    
    // clear error and try a valid operation
    interpreter_clear_error();
    assert(!interpreter_has_error());
    
    ASTNode *valid_node = ast_create_number(42.0);
    double result = interpret(valid_node, env);
    assert(!interpreter_has_error());
    assert(result == 42.0);
    
    ast_destroy(div_node);
    ast_destroy(valid_node);
    env_destroy(env);
    printf("Error recovery test passed\n");
}

void test_interpreter_unknown_operator() {
    printf("Testing unknown binary operator error...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // create a binary operation with invalid operator
    ASTNode *left = ast_create_number(5.0);
    ASTNode *right = ast_create_number(3.0);
    ASTNode *invalid_op = ast_create_binary_op(left, '%', right);  // % is not supported
    
    interpret(invalid_op, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Unknown binary operator: %") != NULL);
    
    ast_destroy(invalid_op);
    env_destroy(env);
    printf("Unknown operator test passed\n");
}

void test_interpret_greater_than() {
    printf("Testing greater than (>) operator...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test 10 > 5 (should return 1.0)
    ASTNode *left = ast_create_number(10.0);
    ASTNode *right = ast_create_number(5.0);
    ASTNode *gt_node = ast_create_binary_op(left, '>', right);
    assert(gt_node != NULL);
    
    double result = interpret(gt_node, env);
    assert(!interpreter_has_error());
    assert(result == 1.0);
    
    ast_destroy(gt_node);
    
    // Test 3 > 5 (should return 0.0)
    ASTNode *left2 = ast_create_number(3.0);
    ASTNode *right2 = ast_create_number(5.0);
    ASTNode *gt_node2 = ast_create_binary_op(left2, '>', right2);
    
    double result2 = interpret(gt_node2, env);
    assert(!interpreter_has_error());
    assert(result2 == 0.0);
    
    ast_destroy(gt_node2);
    env_destroy(env);
    printf("Greater than test passed\n");
}

void test_interpret_less_than() {
    printf("Testing less than (<) operator...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test 3 < 5 (should return 1.0)
    ASTNode *left = ast_create_number(3.0);
    ASTNode *right = ast_create_number(5.0);
    ASTNode *lt_node = ast_create_binary_op(left, '<', right);
    assert(lt_node != NULL);
    
    double result = interpret(lt_node, env);
    assert(!interpreter_has_error());
    assert(result == 1.0);
    
    ast_destroy(lt_node);
    
    // Test 10 < 5 (should return 0.0)
    ASTNode *left2 = ast_create_number(10.0);
    ASTNode *right2 = ast_create_number(5.0);
    ASTNode *lt_node2 = ast_create_binary_op(left2, '<', right2);
    
    double result2 = interpret(lt_node2, env);
    assert(!interpreter_has_error());
    assert(result2 == 0.0);
    
    ast_destroy(lt_node2);
    env_destroy(env);
    printf("Less than test passed\n");
}

void test_interpret_greater_equal() {
    printf("Testing greater than or equal (>=) operator...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test 10 >= 5 (should return 1.0)
    ASTNode *left = ast_create_number(10.0);
    ASTNode *right = ast_create_number(5.0);
    ASTNode *ge_node = ast_create_binary_op(left, 'G', right);  // 'G' represents >=
    assert(ge_node != NULL);
    
    double result = interpret(ge_node, env);
    assert(!interpreter_has_error());
    assert(result == 1.0);
    
    ast_destroy(ge_node);
    
    // Test 5 >= 5 (should return 1.0)
    ASTNode *left2 = ast_create_number(5.0);
    ASTNode *right2 = ast_create_number(5.0);
    ASTNode *ge_node2 = ast_create_binary_op(left2, 'G', right2);
    
    double result2 = interpret(ge_node2, env);
    assert(!interpreter_has_error());
    assert(result2 == 1.0);
    
    ast_destroy(ge_node2);
    
    // Test 3 >= 5 (should return 0.0)
    ASTNode *left3 = ast_create_number(3.0);
    ASTNode *right3 = ast_create_number(5.0);
    ASTNode *ge_node3 = ast_create_binary_op(left3, 'G', right3);
    
    double result3 = interpret(ge_node3, env);
    assert(!interpreter_has_error());
    assert(result3 == 0.0);
    
    ast_destroy(ge_node3);
    env_destroy(env);
    printf("Greater than or equal test passed\n");
}

void test_interpret_less_equal() {
    printf("Testing less than or equal (<=) operator...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test 3 <= 5 (should return 1.0)
    ASTNode *left = ast_create_number(3.0);
    ASTNode *right = ast_create_number(5.0);
    ASTNode *le_node = ast_create_binary_op(left, 'L', right);  // 'L' represents <=
    assert(le_node != NULL);
    
    double result = interpret(le_node, env);
    assert(!interpreter_has_error());
    assert(result == 1.0);
    
    ast_destroy(le_node);
    
    // Test 2 <= 2 (should return 1.0)
    ASTNode *left2 = ast_create_number(2.0);
    ASTNode *right2 = ast_create_number(2.0);
    ASTNode *le_node2 = ast_create_binary_op(left2, 'L', right2);
    
    double result2 = interpret(le_node2, env);
    assert(!interpreter_has_error());
    assert(result2 == 1.0);
    
    ast_destroy(le_node2);
    
    // Test 10 <= 5 (should return 0.0)
    ASTNode *left3 = ast_create_number(10.0);
    ASTNode *right3 = ast_create_number(5.0);
    ASTNode *le_node3 = ast_create_binary_op(left3, 'L', right3);
    
    double result3 = interpret(le_node3, env);
    assert(!interpreter_has_error());
    assert(result3 == 0.0);
    
    ast_destroy(le_node3);
    env_destroy(env);
    printf("Less than or equal test passed\n");
}

void test_interpret_equal() {
    printf("Testing equality (==) operator...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test 5 == 5 (should return 1.0)
    ASTNode *left = ast_create_number(5.0);
    ASTNode *right = ast_create_number(5.0);
    ASTNode *eq_node = ast_create_binary_op(left, 'E', right);  // 'E' represents ==
    assert(eq_node != NULL);
    
    double result = interpret(eq_node, env);
    assert(!interpreter_has_error());
    assert(result == 1.0);
    
    ast_destroy(eq_node);
    
    // Test 3 == 4 (should return 0.0)
    ASTNode *left2 = ast_create_number(3.0);
    ASTNode *right2 = ast_create_number(4.0);
    ASTNode *eq_node2 = ast_create_binary_op(left2, 'E', right2);
    
    double result2 = interpret(eq_node2, env);
    assert(!interpreter_has_error());
    assert(result2 == 0.0);
    
    ast_destroy(eq_node2);
    env_destroy(env);
    printf("Equality test passed\n");
}

void test_interpret_not_equal() {
    printf("Testing not equal (!=) operator...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test 5 != 3 (should return 1.0)
    ASTNode *left = ast_create_number(5.0);
    ASTNode *right = ast_create_number(3.0);
    ASTNode *ne_node = ast_create_binary_op(left, 'N', right);  // 'N' represents !=
    assert(ne_node != NULL);
    
    double result = interpret(ne_node, env);
    assert(!interpreter_has_error());
    assert(result == 1.0);
    
    ast_destroy(ne_node);
    
    // Test 5 != 5 (should return 0.0)
    ASTNode *left2 = ast_create_number(5.0);
    ASTNode *right2 = ast_create_number(5.0);
    ASTNode *ne_node2 = ast_create_binary_op(left2, 'N', right2);
    
    double result2 = interpret(ne_node2, env);
    assert(!interpreter_has_error());
    assert(result2 == 0.0);
    
    ast_destroy(ne_node2);
    env_destroy(env);
    printf("Not equal test passed\n");
}

void test_interpret_floating_point_comparisons() {
    printf("Testing floating-point comparison edge cases...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test 5.0 == 5 (should return 1.0)
    ASTNode *left = ast_create_number(5.0);
    ASTNode *right = ast_create_number(5.0);
    ASTNode *eq_node = ast_create_binary_op(left, 'E', right);
    
    double result = interpret(eq_node, env);
    assert(!interpreter_has_error());
    assert(result == 1.0);
    
    ast_destroy(eq_node);
    
    // Test very small numbers
    ASTNode *small1 = ast_create_number(0.000001);
    ASTNode *small2 = ast_create_number(0.000002);
    ASTNode *lt_small = ast_create_binary_op(small1, '<', small2);
    
    double result2 = interpret(lt_small, env);
    assert(!interpreter_has_error());
    assert(result2 == 1.0);
    
    ast_destroy(lt_small);
    
    // Test very large numbers
    ASTNode *large1 = ast_create_number(1000000.0);
    ASTNode *large2 = ast_create_number(999999.0);
    ASTNode *gt_large = ast_create_binary_op(large1, '>', large2);
    
    double result3 = interpret(gt_large, env);
    assert(!interpreter_has_error());
    assert(result3 == 1.0);
    
    ast_destroy(gt_large);
    
    // Test negative numbers
    ASTNode *neg1 = ast_create_number(-5.0);
    ASTNode *neg2 = ast_create_number(-3.0);
    ASTNode *lt_neg = ast_create_binary_op(neg1, '<', neg2);
    
    double result4 = interpret(lt_neg, env);
    assert(!interpreter_has_error());
    assert(result4 == 1.0);  // -5 < -3 is true
    
    ast_destroy(lt_neg);
    env_destroy(env);
    printf("Floating-point comparison edge cases test passed\n");
}

void test_interpret_comparison_with_variables() {
    printf("Testing comparison operators with variables...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Set variables x = 10, y = 5
    assert(env_set(env, "x", 10.0));
    assert(env_set(env, "y", 5.0));
    
    // Test x > y (should return 1.0)
    ASTNode *x_node = ast_create_identifier("x");
    ASTNode *y_node = ast_create_identifier("y");
    ASTNode *gt_node = ast_create_binary_op(x_node, '>', y_node);
    
    double result = interpret(gt_node, env);
    assert(!interpreter_has_error());
    assert(result == 1.0);
    
    ast_destroy(gt_node);
    
    // Test y == 5 (should return 1.0)
    ASTNode *y_node2 = ast_create_identifier("y");
    ASTNode *five = ast_create_number(5.0);
    ASTNode *eq_node = ast_create_binary_op(y_node2, 'E', five);
    
    double result2 = interpret(eq_node, env);
    assert(!interpreter_has_error());
    assert(result2 == 1.0);
    
    ast_destroy(eq_node);
    env_destroy(env);
    printf("Comparison with variables test passed\n");
}

void test_interpret_complex_comparison_expressions() {
    printf("Testing complex expressions with comparisons...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test (5 + 5) >= 10 (should return 1.0)
    ASTNode *five1 = ast_create_number(5.0);
    ASTNode *five2 = ast_create_number(5.0);
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *add_node = ast_create_binary_op(five1, '+', five2);
    ASTNode *ge_node = ast_create_binary_op(add_node, 'G', ten);
    
    double result = interpret(ge_node, env);
    assert(!interpreter_has_error());
    assert(result == 1.0);
    
    ast_destroy(ge_node);
    
    // Test (2 + 3) == 5 (should return 1.0)
    ASTNode *two = ast_create_number(2.0);
    ASTNode *three = ast_create_number(3.0);
    ASTNode *five3 = ast_create_number(5.0);
    ASTNode *add_node2 = ast_create_binary_op(two, '+', three);
    ASTNode *eq_node = ast_create_binary_op(add_node2, 'E', five3);
    
    double result2 = interpret(eq_node, env);
    assert(!interpreter_has_error());
    assert(result2 == 1.0);
    
    ast_destroy(eq_node);
    
    // Test 2 * 4 > 5 + 2 (should return 1.0, since 8 > 7)
    ASTNode *two2 = ast_create_number(2.0);
    ASTNode *four = ast_create_number(4.0);
    ASTNode *five4 = ast_create_number(5.0);
    ASTNode *two3 = ast_create_number(2.0);
    ASTNode *mul_node = ast_create_binary_op(two2, '*', four);
    ASTNode *add_node3 = ast_create_binary_op(five4, '+', two3);
    ASTNode *gt_node = ast_create_binary_op(mul_node, '>', add_node3);
    
    double result3 = interpret(gt_node, env);
    assert(!interpreter_has_error());
    assert(result3 == 1.0);
    
    ast_destroy(gt_node);
    env_destroy(env);
    printf("Complex comparison expressions test passed\n");
}

void test_interpret_comparison_error_propagation() {
    printf("Testing comparison error propagation...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test comparison with undefined variable on left side
    ASTNode *undefined_left = ast_create_identifier("undefined_var");
    ASTNode *five = ast_create_number(5.0);
    ASTNode *gt_node = ast_create_binary_op(undefined_left, '>', five);
    
    interpret(gt_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Undefined variable: undefined_var") != NULL);
    
    ast_destroy(gt_node);
    interpreter_clear_error();
    
    // Test comparison with undefined variable on right side
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *undefined_right = ast_create_identifier("undefined_var2");
    ASTNode *lt_node = ast_create_binary_op(ten, '<', undefined_right);
    
    interpret(lt_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Undefined variable: undefined_var2") != NULL);
    
    ast_destroy(lt_node);
    interpreter_clear_error();
    
    // Test comparison with division by zero on left side
    ASTNode *five2 = ast_create_number(5.0);
    ASTNode *zero = ast_create_number(0.0);
    ASTNode *three = ast_create_number(3.0);
    ASTNode *div_node = ast_create_binary_op(five2, '/', zero);
    ASTNode *ge_node = ast_create_binary_op(div_node, 'G', three);
    
    interpret(ge_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Division by zero") != NULL);
    
    ast_destroy(ge_node);
    interpreter_clear_error();
    
    // Test comparison with division by zero on right side
    ASTNode *seven = ast_create_number(7.0);
    ASTNode *eight = ast_create_number(8.0);
    ASTNode *zero2 = ast_create_number(0.0);
    ASTNode *div_node2 = ast_create_binary_op(eight, '/', zero2);
    ASTNode *le_node = ast_create_binary_op(seven, 'L', div_node2);
    
    interpret(le_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Division by zero") != NULL);
    
    ast_destroy(le_node);
    interpreter_clear_error();
    
    // Test complex expression with error in comparison subexpression
    ASTNode *two = ast_create_number(2.0);
    ASTNode *undefined_var = ast_create_identifier("missing_var");
    ASTNode *four = ast_create_number(4.0);
    ASTNode *add_node = ast_create_binary_op(two, '+', undefined_var);
    ASTNode *eq_node = ast_create_binary_op(add_node, 'E', four);
    
    interpret(eq_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Undefined variable: missing_var") != NULL);
    
    ast_destroy(eq_node);
    interpreter_clear_error();
    
    env_destroy(env);
    printf("Comparison error propagation test passed\n");
}

void test_interpret_comparison_with_print_errors() {
    printf("Testing comparison errors in print statements...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test print with comparison containing undefined variable
    ASTNode *undefined_var = ast_create_identifier("nonexistent");
    ASTNode *five = ast_create_number(5.0);
    ASTNode *ne_node = ast_create_binary_op(undefined_var, 'N', five);
    ASTNode *print_node = ast_create_print_call(ne_node);
    
    interpret(print_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Undefined variable: nonexistent") != NULL);
    
    ast_destroy(print_node);
    interpreter_clear_error();
    
    // Test print with comparison containing division by zero
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *zero = ast_create_number(0.0);
    ASTNode *three = ast_create_number(3.0);
    ASTNode *div_node = ast_create_binary_op(ten, '/', zero);
    ASTNode *gt_node = ast_create_binary_op(div_node, '>', three);
    ASTNode *print_node2 = ast_create_print_call(gt_node);
    
    interpret(print_node2, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Division by zero") != NULL);
    
    ast_destroy(print_node2);
    interpreter_clear_error();
    
    env_destroy(env);
    printf("Comparison errors in print statements test passed\n");
}

void test_interpret_comparison_let_declaration_errors() {
    printf("Testing comparison errors in let declarations...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test let declaration with comparison containing undefined variable
    ASTNode *undefined_var = ast_create_identifier("unknown_var");
    ASTNode *seven = ast_create_number(7.0);
    ASTNode *lt_node = ast_create_binary_op(undefined_var, '<', seven);
    ASTNode *let_node = ast_create_let_decl("result", lt_node);
    
    interpret(let_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Undefined variable: unknown_var") != NULL);
    
    // Verify the variable was not stored due to error
    double value;
    assert(!env_get(env, "result", &value));
    
    ast_destroy(let_node);
    interpreter_clear_error();
    
    // Test let declaration with comparison containing arithmetic error
    ASTNode *six = ast_create_number(6.0);
    ASTNode *zero = ast_create_number(0.0);
    ASTNode *four = ast_create_number(4.0);
    ASTNode *div_node = ast_create_binary_op(six, '/', zero);
    ASTNode *ge_node = ast_create_binary_op(div_node, 'G', four);
    ASTNode *let_node2 = ast_create_let_decl("comparison_result", ge_node);
    
    interpret(let_node2, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Division by zero") != NULL);
    
    // Verify the variable was not stored due to error
    assert(!env_get(env, "comparison_result", &value));
    
    ast_destroy(let_node2);
    interpreter_clear_error();
    
    env_destroy(env);
    printf("Comparison errors in let declarations test passed\n");
}

void test_interpret_nested_comparison_errors() {
    printf("Testing nested comparison expression errors...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // Test deeply nested expression with error: ((5 + undefined_var) > 3) == 1
    ASTNode *five = ast_create_number(5.0);
    ASTNode *undefined_var = ast_create_identifier("deep_undefined");
    ASTNode *three = ast_create_number(3.0);
    ASTNode *one = ast_create_number(1.0);
    
    ASTNode *add_node = ast_create_binary_op(five, '+', undefined_var);
    ASTNode *gt_node = ast_create_binary_op(add_node, '>', three);
    ASTNode *eq_node = ast_create_binary_op(gt_node, 'E', one);
    
    interpret(eq_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Undefined variable: deep_undefined") != NULL);
    
    ast_destroy(eq_node);
    interpreter_clear_error();
    
    // Test comparison chain with error: (5 > undefined_var) != (3 < 4)
    ASTNode *five2 = ast_create_number(5.0);
    ASTNode *undefined_var2 = ast_create_identifier("chain_undefined");
    ASTNode *three2 = ast_create_number(3.0);
    ASTNode *four = ast_create_number(4.0);
    
    ASTNode *gt_node2 = ast_create_binary_op(five2, '>', undefined_var2);
    ASTNode *lt_node = ast_create_binary_op(three2, '<', four);
    ASTNode *ne_node = ast_create_binary_op(gt_node2, 'N', lt_node);
    
    interpret(ne_node, env);
    assert(interpreter_has_error());
    assert(strstr(interpreter_get_error(), "Undefined variable: chain_undefined") != NULL);
    
    ast_destroy(ne_node);
    interpreter_clear_error();
    
    env_destroy(env);
    printf("Nested comparison expression errors test passed\n");
}

void test_interpreter_comprehensive_scenario() {
    printf("Testing comprehensive interpreter scenario...\n");
    
    Environment *env = env_create();
    assert(env != NULL);
    
    // scenario: let x = 10; let y = x * 2; print(y + 5);
    
    // step 1: let x = 10;
    ASTNode *ten = ast_create_number(10.0);
    ASTNode *let_x = ast_create_let_decl("x", ten);
    
    double result1 = interpret(let_x, env);
    assert(!interpreter_has_error());
    assert(result1 == 10.0);
    
    // step 2: let y = x * 2;
    ASTNode *x_ref = ast_create_identifier("x");
    ASTNode *two = ast_create_number(2.0);
    ASTNode *mul_node = ast_create_binary_op(x_ref, '*', two);
    ASTNode *let_y = ast_create_let_decl("y", mul_node);
    
    double result2 = interpret(let_y, env);
    assert(!interpreter_has_error());
    assert(result2 == 20.0);
    
    // step 3: print(y + 5);
    ASTNode *y_ref = ast_create_identifier("y");
    ASTNode *five = ast_create_number(5.0);
    ASTNode *add_node = ast_create_binary_op(y_ref, '+', five);
    ASTNode *print_node = ast_create_print_call(add_node);
    
    printf("Expected output: 25\nActual output: ");
    double result3 = interpret(print_node, env);
    assert(!interpreter_has_error());
    assert(result3 == 25.0);
    
    // verify final environment state
    double x_val, y_val;
    assert(env_get(env, "x", &x_val));
    assert(env_get(env, "y", &y_val));
    assert(x_val == 10.0);
    assert(y_val == 20.0);
    
    ast_destroy(let_x);
    ast_destroy(let_y);
    ast_destroy(print_node);
    env_destroy(env);
    printf("Comprehensive scenario test passed\n");
}

int main() {
    printf("Running interpreter core tests...\n\n");
    
    test_interpret_number();
    test_interpret_identifier_success();
    test_interpret_identifier_undefined();
    test_interpret_null_node();
    test_interpret_null_env();
    
    printf("\nRunning arithmetic expression tests...\n\n");
    
    test_interpret_addition();
    test_interpret_subtraction();
    test_interpret_multiplication();
    test_interpret_division();
    test_interpret_division_by_zero();
    test_interpret_complex_expression();
    test_interpret_expression_with_variables();
    
    printf("\nRunning comparison operator tests...\n\n");
    
    test_interpret_greater_than();
    test_interpret_less_than();
    test_interpret_greater_equal();
    test_interpret_less_equal();
    test_interpret_equal();
    test_interpret_not_equal();
    test_interpret_floating_point_comparisons();
    test_interpret_comparison_with_variables();
    test_interpret_complex_comparison_expressions();
    
    printf("\nRunning variable declaration tests...\n\n");
    
    test_interpret_let_declaration();
    test_interpret_let_with_expression();
    test_interpret_let_redeclaration();
    test_interpret_let_with_variable_reference();
    
    printf("\nRunning print function tests...\n\n");
    
    test_interpret_print_number();
    test_interpret_print_expression();
    test_interpret_print_variable();
    test_interpret_print_undefined_variable();
    
    printf("\nRunning comprehensive error handling tests...\n\n");
    
    test_interpreter_error_propagation();
    test_interpreter_division_by_zero_in_expression();
    test_interpreter_let_with_error();
    test_interpreter_error_recovery();
    test_interpreter_unknown_operator();
    
    printf("\nRunning comparison operator error handling tests...\n\n");
    
    test_interpret_comparison_error_propagation();
    test_interpret_comparison_with_print_errors();
    test_interpret_comparison_let_declaration_errors();
    test_interpret_nested_comparison_errors();
    
    test_interpreter_comprehensive_scenario();
    
    printf("All interpreter tests passed!\n");
    return 0;
}