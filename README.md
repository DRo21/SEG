
# SEG Language Compiler

SEG (Simple Executable Grammar) is a **minimal C-like programming language** designed for learning compiler construction in C.

This project includes:
- A **Lexer** to tokenize SEG source code.
- A **Parser** to build an Abstract Syntax Tree (AST) for variable declarations and arithmetic expressions.
- A **Symbol Table** to track variable names and types.
- A **Code Generator** that emits x86-64 assembly code for SEG programs.
- A **Minimal Runtime Model**: SEG programs generate assembly that can be compiled and executed using GCC.

---

## Example SEG Code

```c
int x = 5;
float y = 3.14;
int z = (5 + 3) * 2;
float a = 3.14 + (x * y);
```

---

## Building SEG

### Prerequisites
- A C compiler (e.g., GCC, Clang)
- CMake (>=3.10)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/DRo21/SEG
cd SEG

# Build the project
mkdir build
cd build
cmake ..
make
```

---

## Running the Compiler

```bash
./seg ../tests/test1.seg
```

This will generate `output.s`, an x86-64 assembly file. You can compile it with GCC:

```bash
gcc -m64 output.s -o program
```

Run the compiled program and check the return value:

```bash
./program
echo $?
```

---

## Current Features

- Supports `int` and `float` variable declarations.
- Supports arithmetic expressions: `+`, `-`, `*`, `/`, with correct operator precedence and parentheses.
- Generates x86-64 assembly code using Intel syntax.
- Symbol table implementation for tracking declared variables.
- Last declared variable's value is returned as the program's exit code.

---

## Future Work

- Add type checking for `int` and `float`.
- Support for `float` code generation (SSE/AVX).
- Implement functions, control flow (if/else, loops).
- Add a standard library (e.g., `print`, I/O functions).

---

## Author

Dario Romandini

---

## License

MIT License
