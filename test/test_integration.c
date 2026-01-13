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
    
    printf("\nComparison Operator Tests:\n");
    printf("=========================\n\n");
    
    // test 9: basic comparison operators
    if (run_test_script("print(1 < 2);", "1\n", "Less than - true")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(3 > 5);", "0\n", "Greater than - false")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(2 <= 2);", "1\n", "Less than or equal - true")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(5 >= 3);", "1\n", "Greater than or equal - true")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(5 == 5);", "1\n", "Equal - true")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(5 != 5);", "0\n", "Not equal - false")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 10: complex comparison expressions
    if (run_test_script("print((5 + 5) >= 10);", "1\n", "Complex expression - (5 + 5) >= 10")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print((2 + 3) == 5);", "1\n", "Complex expression - (2 + 3) == 5")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print((3 * 2) != (4 + 2));", "0\n", "Complex expression - (3 * 2) != (4 + 2)")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(5 + 3 > 2 * 4);", "0\n", "Precedence test - 5 + 3 > 2 * 4")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 11: variable comparisons
    if (run_test_script("let x = 5;\nprint(x > 3);", "1\n", "Variable comparison - x > 3")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("let x = 10;\nlet y = 5;\nprint(x >= y);", "1\n", "Two variable comparison - x >= y")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("let x = 7;\nlet y = 3;\nprint((x + y) == 10);", "1\n", "Variable arithmetic comparison - (x + y) == 10")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 12: mixed arithmetic-comparison expressions
    if (run_test_script("let x = 4;\nprint(x * 2 > x + 3);", "1\n", "Mixed expression - x * 2 > x + 3")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("let a = 2;\nlet b = 3;\nprint((a + b) * 2 <= 10);", "1\n", "Complex mixed expression - (a + b) * 2 <= 10")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print((5 + 5) >= (2 * 5));", "1\n", "Parenthesized comparison - (5 + 5) >= (2 * 5)")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    // test 13: floating point comparisons
    if (run_test_script("print(3.14 > 3);", "1\n", "Floating point comparison - 3.14 > 3")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_test_script("print(5.0 == 5);", "1\n", "Float-int equality - 5.0 == 5")) {
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
    
    printf("\nComparison Operator Error Tests:\n");
    printf("===============================\n\n");
    
    // test comparison operator error conditions
    if (run_error_test("print(undefined_var > 5);", "Runtime error - undefined variable in comparison")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_error_test("print((5 / 0) >= 3);", "Runtime error - division by zero in comparison")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_error_test("let x = (unknown_var < 10);", "Runtime error - undefined variable in let with comparison")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_error_test("print((5 + undefined_var) == 10);", "Runtime error - undefined variable in complex comparison")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_error_test("let result = (10 / 0) != 5;", "Runtime error - division by zero in comparison assignment")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_error_test("5 >=;", "Syntax error - incomplete comparison expression")) {
        results.passed++;
    } else {
        results.failed++;
    }
    
    if (run_error_test("print(5 > );", "Syntax error - missing right operand in comparison")) {
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