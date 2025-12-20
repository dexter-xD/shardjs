/*
 * env.c - variable environment for shardjs interpreter
 * 
 * handles variable storage and lookup using a dynamic array.
 * supports creating, updating, and retrieving variables by name.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/runtime.h"

// simple name-value pair for variables
typedef struct {
    char *name;
    double value;
} Variable;

// environment holds all variables in a resizable array
struct Environment {
    Variable *variables;
    size_t count;
    size_t capacity;
};

#define INITIAL_CAPACITY 8

// double the array size when we run out of space
static int env_resize(Environment *env) {
    size_t new_capacity = env->capacity * 2;
    Variable *new_variables = realloc(env->variables, new_capacity * sizeof(Variable));
    if (!new_variables) {
        return 0; // out of memory
    }
    env->variables = new_variables;
    env->capacity = new_capacity;
    return 1;
}

// create new environment with initial capacity
Environment* env_create(void) {
    Environment *env = malloc(sizeof(Environment));
    if (!env) {
        return NULL;
    }
    
    env->variables = malloc(INITIAL_CAPACITY * sizeof(Variable));
    if (!env->variables) {
        free(env);
        return NULL;
    }
    
    env->count = 0;
    env->capacity = INITIAL_CAPACITY;
    return env;
}

// cleanup all memory used by environment
void env_destroy(Environment *env) {
    if (!env) {
        return;
    }
    
    // free variable names first
    for (size_t i = 0; i < env->count; i++) {
        free(env->variables[i].name);
    }
    
    free(env->variables);
    free(env);
}

// set variable value - creates new or updates existing
int env_set(Environment *env, const char *name, double value) {
    if (!env || !name) {
        return 0;
    }
    
    // check if var already exists
    for (size_t i = 0; i < env->count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            env->variables[i].value = value;
            return 1;
        }
    }
    
    // need more space?
    if (env->count >= env->capacity) {
        if (!env_resize(env)) {
            return 0;
        }
    }
    
    // copy the name string
    char *name_copy = malloc(strlen(name) + 1);
    if (!name_copy) {
        return 0;
    }
    strcpy(name_copy, name);
    
    // add new variable
    env->variables[env->count].name = name_copy;
    env->variables[env->count].value = value;
    env->count++;
    
    return 1;
}

// get variable value by name
int env_get(Environment *env, const char *name, double *value) {
    if (!env || !name || !value) {
        return 0;
    }
    
    // linear search through variables
    for (size_t i = 0; i < env->count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            *value = env->variables[i].value;
            return 1;
        }
    }
    
    return 0; // not found
}