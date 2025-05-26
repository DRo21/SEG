cmake_minimum_required(VERSION 3.10)
project(SEG LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# Include directories
include_directories(include)

# Source files
set(SOURCES
    src/main.c
    src/lexer.c
    src/parser.c
    src/ast.c
)

# Executable
add_executable(seg ${SOURCES})
