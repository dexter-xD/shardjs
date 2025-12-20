CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

# directories
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = test
INCLUDE_DIR = include

# targets
TARGET = $(BIN_DIR)/shardjs
TEST_LEXER_TARGET = $(BIN_DIR)/test_lexer
TEST_PARSER_TARGET = $(BIN_DIR)/test_parser
TEST_AST_TARGET = $(BIN_DIR)/test_ast
TEST_ENV_TARGET = $(BIN_DIR)/test_env
TEST_INTERPRETER_TARGET = $(BIN_DIR)/test_interpreter
TEST_INTEGRATION_TARGET = $(BIN_DIR)/test_integration

# sources
SOURCES = main.c lexer.c parser.c ast.c env.c interpreter.c
TEST_LEXER_SOURCES = $(TEST_DIR)/test_lexer.c lexer.c ast.c env.c interpreter.c
TEST_PARSER_SOURCES = $(TEST_DIR)/test_parser.c lexer.c parser.c ast.c env.c interpreter.c
TEST_AST_SOURCES = $(TEST_DIR)/test_ast.c ast.c env.c interpreter.c
TEST_ENV_SOURCES = $(TEST_DIR)/test_env.c env.c interpreter.c
TEST_INTERPRETER_SOURCES = $(TEST_DIR)/test_interpreter.c ast.c env.c interpreter.c
TEST_INTEGRATION_SOURCES = $(TEST_DIR)/test_integration.c

# objects with build directory
OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)

# test object files
TEST_LEXER_OBJECTS = $(BUILD_DIR)/test_lexer.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/ast.o $(BUILD_DIR)/env.o $(BUILD_DIR)/interpreter.o
TEST_PARSER_OBJECTS = $(BUILD_DIR)/test_parser.o $(BUILD_DIR)/lexer.o $(BUILD_DIR)/parser.o $(BUILD_DIR)/ast.o $(BUILD_DIR)/env.o $(BUILD_DIR)/interpreter.o
TEST_AST_OBJECTS = $(BUILD_DIR)/test_ast.o $(BUILD_DIR)/ast.o $(BUILD_DIR)/env.o $(BUILD_DIR)/interpreter.o
TEST_ENV_OBJECTS = $(BUILD_DIR)/test_env.o $(BUILD_DIR)/env.o $(BUILD_DIR)/interpreter.o
TEST_INTERPRETER_OBJECTS = $(BUILD_DIR)/test_interpreter.o $(BUILD_DIR)/ast.o $(BUILD_DIR)/env.o $(BUILD_DIR)/interpreter.o
TEST_INTEGRATION_OBJECTS = $(BUILD_DIR)/test_integration.o

.PHONY: all clean test dirs

all: dirs $(TARGET)

dirs:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_LEXER_TARGET): $(TEST_LEXER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_PARSER_TARGET): $(TEST_PARSER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_AST_TARGET): $(TEST_AST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_ENV_TARGET): $(TEST_ENV_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_INTERPRETER_TARGET): $(TEST_INTERPRETER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_INTEGRATION_TARGET): $(TEST_INTEGRATION_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

test: dirs $(TEST_LEXER_TARGET) $(TEST_PARSER_TARGET) $(TEST_AST_TARGET) $(TEST_ENV_TARGET) $(TEST_INTERPRETER_TARGET) $(TARGET) $(TEST_INTEGRATION_TARGET)
	@echo "Running lexer tests..."
	$(TEST_LEXER_TARGET)
	@echo "Running parser tests..."
	$(TEST_PARSER_TARGET)
	@echo "Running AST tests..."
	$(TEST_AST_TARGET)
	@echo "Running environment tests..."
	$(TEST_ENV_TARGET)
	@echo "Running interpreter tests..."
	$(TEST_INTERPRETER_TARGET)
	@echo "Running integration tests..."
	$(TEST_INTEGRATION_TARGET)

# dependencies
$(BUILD_DIR)/main.o: main.c $(INCLUDE_DIR)/token.h $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/lexer.o: lexer.c $(INCLUDE_DIR)/token.h $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/parser.o: parser.c $(INCLUDE_DIR)/token.h $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/ast.o: ast.c $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/env.o: env.c $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/interpreter.o: interpreter.c $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/test_lexer.o: $(TEST_DIR)/test_lexer.c $(INCLUDE_DIR)/token.h $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/test_parser.o: $(TEST_DIR)/test_parser.c $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/test_ast.o: $(TEST_DIR)/test_ast.c $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/test_env.o: $(TEST_DIR)/test_env.c $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/test_interpreter.o: $(TEST_DIR)/test_interpreter.c $(INCLUDE_DIR)/runtime.h
$(BUILD_DIR)/test_integration.o: $(TEST_DIR)/test_integration.c