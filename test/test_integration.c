/*
 * test_integration.c - end-to-end integration tests
 * 
 * runs complete shardjs programs and verifies output.
 * tests both successful execution and error conditions.
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// test result tracking
typedef struct {
    int passed;
    int failed;
} TestResults;

// run test script and capture output
int run_test_script(const char *script_content, const char *expected_output, const char *test_name) {
    // write script to temp file
    FILE *script_file = fopen("temp_test.js", "w");
    if (!script_file) {
        printf("FAIL: %s - Could not create temporary script file\n", test_name);
        return 0;
    }
    fprintf(script_file, "%s", script_content);
    fclose(script_file);
    
    // run the shardjs interpreter
    FILE *output = popen("./bin/shardjs temp_test.js 2>&1", "r");
    if (!output) {
        printf("FAIL: %s - Could not run shardjs\n", test_name);
        unlink("temp_test.js");
        return 0;
    }
    
    // capture output
    char buffer[1024] = {0};
    size_t total_read = 0;
    size_t bytes_read;
    while ((bytes_read = fread(buffer + total_read, 1, sizeof(buffer) - total_read - 1, output)) > 0) {
        total_read += bytes_read;
    }
    
    int exit_status = pclose(output);
    unlink("temp_test.js");
    
    // compare output
    if (strcmp(buffer, expected_output) == 0) {
        printf("PASS: %s\n", test_name);
        return 1;
    } else {
        printf("FAIL: %s\n", test_name);
        printf("  Expected: '%s'\n", expected_output);
        printf("  Got:      '%s'\n", buffer);
        printf("  Exit status: %d\n", exit_status);
        return 0;
    }
}

// test error conditions
int run_error_test(const char *script_content, const char *test_name) {
    // write script to temp file
    FILE *script_file = fopen("temp_test.js", "w");
    if (!script_file) {
        printf("FAIL: %s - Could not create temporary script file\n", test_name);
        return 0;
    }
    fprintf(script_file, "%s", script_content);
    fclose(script_file);
    
    // run the shardjs interpreter
    int exit_status = system("./bin/shardjs temp_test.js > /dev/null 2>&1");
    unlink("temp_test.js");
    
    // check that it failed (non-zero exit status)
    if (exit_status != 0) {
        printf("PASS: %s (correctly failed)\n", test_name);
        return 1;
    } else {
        printf("FAIL: %s (should have failed but didn't)\n", test_name);
        return 0;
    }
}

int main() {
    TestResults results = {0, 0};
    
    printf("Running ShardJS Integration Tests\n");
    printf("=================================\n\n");
    
    // test 1: basic number printing
    if (run_test_script("print(42);", "42\n", "Basic number printing")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 2: arithmetic expressions
    if (run_test_script("print(5 + 3);", "8\n", "Addition")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(10 - 4);", "6\n", "Subtraction")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(6 * 7);", "42\n", "Multiplication")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(15 / 3);", "5\n", "Division")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 3: operator precedence
    if (run_test_script("print(2 + 3 * 4);", "14\n", "Operator precedence (* before +)")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print((2 + 3) * 4);", "20\n", "Parentheses precedence")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 4: variable declarations and usage
    if (run_test_script("let x = 5;\nprint(x);", "5\n", "Variable declaration and usage")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("let x = 5 + 3;\nprint(x);", "8\n", "Variable with expression")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("let x = 5;\nlet y = x * 2;\nprint(y);", "10\n", "Variable referencing another variable")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 5: variable redeclaration
    if (run_test_script("let x = 5;\nlet x = 10;\nprint(x);", "10\n", "Variable redeclaration")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 6: multiple statements
    if (run_test_script("let x = 5;\nprint(x);\nlet y = x + 3;\nprint(y);", "5\n8\n", "Multiple statements")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 7: complex expressions
    if (run_test_script("let x = 2;\nlet y = 3;\nprint(x * y + 1);", "7\n", "Complex expression with variables")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 8: floating point numbers
    if (run_test_script("print(3.14);", "3.14\n", "Floating point literal")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(1.5 + 2.5);", "4\n", "Floating point arithmetic")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    printf("\nError Condition Tests:\n");
    printf("=====================\n\n");
    
    // test error conditions
    if (run_error_test("let x = 5 +;", "Syntax error - incomplete expression")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_error_test("print(undefined_var);", "Runtime error - undefined variable")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_error_test("let x = 5\nprint(x", "Syntax error - missing semicolon and parenthesis")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_error_test("print(5 / 0);", "Runtime error - division by zero")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // print results
    printf("\n=================================\n");
    printf("Test Results: %d passed, %d failed\n", results.passed, results.failed);
    
    if (results.failed == 0) {
        printf("All tests passed!\n");
        return 0;
    } else {
        printf("Some tests failed.\n");
        return 1;
    }
}