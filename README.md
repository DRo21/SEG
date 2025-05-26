# SEG Language Compiler

SEG (Simple Executable Grammar) is a **minimal programming language** inspired by C, designed for learning compiler construction in C.

This project includes:
- A **Lexer** to tokenize SEG source code.
- A **Parser** to build an Abstract Syntax Tree (AST) for variable declarations.
- A basic **AST representation** for variable declarations.

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

Prerequisites
- A C compiler (e.g., GCC, Clang)
- CMake (>=3.10)

## Build Instructions

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
## Run

```bash
Run
bash
Copy
Edit
./seg ../tests/test1.seg