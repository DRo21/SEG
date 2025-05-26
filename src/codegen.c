/**
 * @file codegen.c
 * @brief Code generator implementation for the SEG compiler.
 *        Translates SEG AST into x86-64 assembly.
 *        Supports variable declarations, numeric literals, and arithmetic expressions.
 *        Emits Intel-style syntax for use with GCC (via .intel_syntax noprefix).
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "symbol.h"

static void generate_expression(ASTNode *node, FILE *output, Symbol *symbols);
static void generate_data_section(ASTNode *program, FILE *output, Symbol **symbols);

/**
 * @brief Generate x86-64 assembly code for a SEG program.
 *
 * @param program Pointer to the AST root (linked list of var declarations)
 * @param output File pointer to write assembly output (program.s)
 */
void generate_program(ASTNode *program, FILE *output)
{
    Symbol *symbols = NULL; // Symbol table

    fprintf(output, "    .intel_syntax noprefix\n");

    // Generate .data section
    generate_data_section(program, output, &symbols);

    // Generate .text section
    fprintf(output, "    .text\n");
    fprintf(output, "    .global main\n");
    fprintf(output, "main:\n");

    ASTNode *current = program;
    ASTNode *last_var_decl = NULL;

    while (current)
    {
        if (current->type == AST_VAR_DECL)
        {
            generate_expression(current->value, output, symbols);
            fprintf(output, "    mov [rip + %s], rax\n", current->name);
            last_var_decl = current; // Track the last variable
        }
        current = current->next;
    }

    // Load the last variable into rax (for return)
    if (last_var_decl)
    {
        fprintf(output, "    mov rax, [rip + %s]\n", last_var_decl->name);
    }
    else
    {
        fprintf(output, "    mov rax, 0\n"); // No variables? Return 0.
    }

    fprintf(output, "    ret\n");

    free_symbol_table(symbols);
}

static void generate_data_section(ASTNode *program, FILE *output, Symbol **symbols)
{
    fprintf(output, "    .data\n");

    ASTNode *current = program;
    while (current)
    {
        if (current->type == AST_VAR_DECL)
        {
            // Add variable to symbol table
            *symbols = add_symbol(*symbols, current->name, current->var_type);
            fprintf(output, "%s: .quad 0\n", current->name);
        }
        current = current->next;
    }
}

static void generate_expression(ASTNode *node, FILE *output, Symbol *symbols)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_NUMBER_LITERAL:
        fprintf(output, "    mov rax, %s\n", node->literal);
        break;

    case AST_IDENTIFIER:
    {
        Symbol *sym = lookup_symbol(symbols, node->name);
        if (!sym)
        {
            fprintf(stderr, "[Codegen Error] Undefined variable: %s\n", node->name);
            exit(1);
        }
        fprintf(output, "    mov rax, [rip + %s]\n", node->name);
        break;
    }

    case AST_BINARY_EXPR:
        generate_expression(node->right, output, symbols);
        fprintf(output, "    push rax\n");
        generate_expression(node->left, output, symbols);
        fprintf(output, "    pop rbx\n");

        switch (node->op)
        {
        case TOKEN_PLUS:
            fprintf(output, "    add rax, rbx\n");
            break;
        case TOKEN_MINUS:
            fprintf(output, "    sub rax, rbx\n");
            break;
        case TOKEN_STAR:
            fprintf(output, "    imul rax, rbx\n");
            break;
        case TOKEN_SLASH:
            fprintf(output, "    cqo\n");
            fprintf(output, "    idiv rbx\n");
            break;
        default:
            fprintf(output, "    ; [unsupported binary op]\n");
        }
        break;

    default:
        fprintf(output, "    ; [unsupported node type]\n");
        break;
    }
}
