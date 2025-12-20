/*
 * test_env.c - tests for variable environment
 * 
 * tests variable storage, retrieval, dynamic resizing,
 * and error handling for the environment system.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../include/runtime.h"

// test helper
static void test_assert(int condition, const char *message) {
    if (!condition) {
        printf("FAIL: %s\n", message);
        exit(1);
    }
    printf("PASS: %s\n", message);
}

// test environment creation and destruction
static void test_env_create_destroy() {
    Environment *env = env_create();
    test_assert(env != NULL, "Environment creation should succeed");
    
    env_destroy(env);
    printf("PASS: Environment destruction completed without crash\n");
}

// test variable storage and retrieval
static void test_env_set_get() {
    Environment *env = env_create();
    test_assert(env != NULL, "Environment creation should succeed");
    
    // test setting a variable
    int result = env_set(env, "x", 42.0);
    test_assert(result == 1, "Setting variable should succeed");
    
    // test getting the variable
    double value;
    result = env_get(env, "x", &value);
    test_assert(result == 1, "Getting existing variable should succeed");
    test_assert(value == 42.0, "Retrieved value should match stored value");
    
    env_destroy(env);
}

// test variable redeclaration (updating existing values)
static void test_env_redeclaration() {
    Environment *env = env_create();
    test_assert(env != NULL, "Environment creation should succeed");
    
    // set initial value
    int result = env_set(env, "y", 10.0);
    test_assert(result == 1, "Setting initial variable should succeed");
    
    // update the value
    result = env_set(env, "y", 20.0);
    test_assert(result == 1, "Updating variable should succeed");
    
    // verify updated value
    double value;
    result = env_get(env, "y", &value);
    test_assert(result == 1, "Getting updated variable should succeed");
    test_assert(value == 20.0, "Updated value should be correct");
    
    env_destroy(env);
}

// test error handling for undefined variables
static void test_env_undefined_variable() {
    Environment *env = env_create();
    test_assert(env != NULL, "Environment creation should succeed");
    
    // try to get undefined variable
    double value;
    int result = env_get(env, "undefined", &value);
    test_assert(result == 0, "Getting undefined variable should fail");
    
    env_destroy(env);
}

// test multiple variables
static void test_env_multiple_variables() {
    Environment *env = env_create();
    test_assert(env != NULL, "Environment creation should succeed");
    
    // set multiple variables
    test_assert(env_set(env, "a", 1.0) == 1, "Setting variable 'a' should succeed");
    test_assert(env_set(env, "b", 2.0) == 1, "Setting variable 'b' should succeed");
    test_assert(env_set(env, "c", 3.0) == 1, "Setting variable 'c' should succeed");
    
    // verify all variables
    double value;
    test_assert(env_get(env, "a", &value) == 1 && value == 1.0, "Variable 'a' should be correct");
    test_assert(env_get(env, "b", &value) == 1 && value == 2.0, "Variable 'b' should be correct");
    test_assert(env_get(env, "c", &value) == 1 && value == 3.0, "Variable 'c' should be correct");
    
    env_destroy(env);
}

// test dynamic array resizing
static void test_env_dynamic_resize() {
    Environment *env = env_create();
    test_assert(env != NULL, "Environment creation should succeed");
    
    // add more variables than initial capacity (8) to test resizing
    char var_name[10];
    for (int i = 0; i < 20; i++) {
        snprintf(var_name, sizeof(var_name), "var%d", i);
        int result = env_set(env, var_name, (double)i);
        test_assert(result == 1, "Setting variable during resize should succeed");
    }
    
    // verify all variables are still accessible
    double value;
    for (int i = 0; i < 20; i++) {
        snprintf(var_name, sizeof(var_name), "var%d", i);
        int result = env_get(env, var_name, &value);
        test_assert(result == 1 && value == (double)i, "Variable should be accessible after resize");
    }
    
    env_destroy(env);
}

// test error handling with null parameters
static void test_env_null_parameters() {
    Environment *env = env_create();
    test_assert(env != NULL, "Environment creation should succeed");
    
    // test env_set with null parameters
    test_assert(env_set(NULL, "test", 1.0) == 0, "env_set with NULL env should fail");
    test_assert(env_set(env, NULL, 1.0) == 0, "env_set with NULL name should fail");
    
    // test env_get with null parameters
    double value;
    test_assert(env_get(NULL, "test", &value) == 0, "env_get with NULL env should fail");
    test_assert(env_get(env, NULL, &value) == 0, "env_get with NULL name should fail");
    test_assert(env_get(env, "test", NULL) == 0, "env_get with NULL value pointer should fail");
    
    env_destroy(env);
}

int main() {
    printf("Running environment tests...\n\n");
    
    test_env_create_destroy();
    test_env_set_get();
    test_env_redeclaration();
    test_env_undefined_variable();
    test_env_multiple_variables();
    test_env_dynamic_resize();
    test_env_null_parameters();
    
    printf("\nAll environment tests passed!\n");
    return 0;
}