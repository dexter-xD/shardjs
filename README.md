<div align="center">

<img src="assets/logo.png" alt="ShardJS Logo" width="128" height="128" style="border-radius: 17px;">

# ShardJS

**A minimal JavaScript-like runtime written in C**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C](https://img.shields.io/badge/C-99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![Platform](https://img.shields.io/badge/Platform-Linux-green.svg)](https://www.linux.org/)
[![Build System](https://img.shields.io/badge/Build-Make-red.svg)](https://www.gnu.org/software/make/)
[![Parser](https://img.shields.io/badge/Parser-Recursive%20Descent-purple.svg)](https://en.wikipedia.org/wiki/Recursive_descent_parser)
[![Memory](https://img.shields.io/badge/Memory-Manual%20Management-orange.svg)](https://en.wikipedia.org/wiki/Manual_memory_management)

</div>

ShardJS supports basic arithmetic, variables, and expression evaluation through a clean interpreter pipeline.

> **⚠️ WARNING**: This is an experimental runtime for educational purposes only. The project is under active development and in very early stages. Not intended for production use.

## Features

- **Numbers**: Double-precision floating point arithmetic
- **Variables**: `let` declarations with assignment
- **Operators**: `+`, `-`, `*`, `/` with proper precedence
- **Parentheses**: Expression grouping support
- **Print function**: Built-in `print()` for output

## Quick Start

```bash
# build the interpreter
make

# run a script
./shardjs script.js
```

## Example Script

```javascript
let x = 10;
let y = 5;
let result = (x + y) * 2;
print(result); // outputs: 30
```

## Architecture

ShardJS follows a traditional interpreter pipeline:

```
Source Code → Lexer → Parser → AST → Interpreter → Output
```

### Pipeline Flow

1. **Lexer** - Tokenizes source code character by character
2. **Parser** - Builds Abstract Syntax Tree using recursive descent
3. **AST** - Represents program structure in memory
4. **Interpreter** - Executes AST nodes and manages variables

### Core Components

```
shardjs/
├── main.c          # entry point and orchestration
├── lexer.c         # tokenization logic
├── parser.c        # recursive descent parser
├── ast.c           # AST node management
├── env.c           # variable environment
├── interpreter.c   # AST execution engine
└── include/
    ├── token.h     # token definitions
    └── runtime.h   # core data structures
```

## Language Grammar

```
program     → statement*
statement   → letDecl | printCall
letDecl     → "let" IDENTIFIER "=" expression ";"
printCall   → "print" "(" expression ")" ";"
expression  → term ( ( "+" | "-" ) term )*
term        → factor ( ( "*" | "/" ) factor )*
factor      → NUMBER | IDENTIFIER | "(" expression ")"
```

## Memory Management

- All components handle their own memory allocation/deallocation
- AST nodes are freed recursively after interpretation
- Environment cleanup handles variable storage
- No external dependencies beyond standard C library

## Building

Requirements:
- GCC or Clang compiler
- Make build system
- Standard C library

```bash
# compile with warnings enabled
make

# run tests
make test

# clean build artifacts
make clean
```

## Error Handling

ShardJS provides clear error messages for:
- **Lexical errors**: Invalid characters with line/column info
- **Parse errors**: Syntax issues with expected vs actual tokens
- **Runtime errors**: Undefined variables, division by zero

## Testing

The project includes comprehensive tests:
- Unit tests for each component
- Integration tests for complete pipeline
- Memory leak detection support
- Error condition validation

Run tests with `make test` to verify functionality.
## 
🤝 Contributing

I welcome contributions from the community! Whether you're fixing bugs, adding features, or improving documentation, your help is appreciated.

**Quick Start:**

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## ☕ Support

If you find ShardJS helpful, consider supporting the project:

[![Buy Me A Coffee](https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png)](https://buymeacoffee.com/trish07)