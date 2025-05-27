/**
 * @file codegen.h
 * @brief Code generator for the SEG compiler.
 *        Generates x86-64 assembly from the SEG AST with type-aware code generation.
 * @author Dario Romandini
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"

/**
 * @brief Generate x86-64 assembly code for a SEG program.
 *
 * @param program Pointer to the AST root (linked list of var declarations)
 * @param output File pointer to write assembly output (e.g., output.s)
 */
void generate_program(ASTNode *program, FILE *output);

#endif // CODEGEN_H
