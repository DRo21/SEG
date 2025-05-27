/**
 * @file codegen.c
 * @brief Code generator implementation for the SEG compiler.
 *        Translates SEG AST into x86-64 assembly.
 *        Supports variable declarations, numeric literals, and arithmetic expressions with type-aware codegen.
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symbol.h"

static int float_literal_counter = 0;

typedef struct FloatLiteral
{
    char *label;
    char *value;
    struct FloatLiteral *next;
} FloatLiteral;

static FloatLiteral *float_literals = NULL;

static void add_float_literal(const char *value)
{
    for (FloatLiteral *lit = float_literals; lit; lit = lit->next)
    {
        if (strcmp(lit->value, value) == 0) return;
    }

    FloatLiteral *lit = (FloatLiteral *)malloc(sizeof(FloatLiteral));
    lit->label = (char *)malloc(32);
    sprintf(lit->label, "L_float_%d", float_literal_counter++);
    lit->value = strdup(value);
    lit->next = float_literals;
    float_literals = lit;
}

static const char *get_float_label(const char *value)
{
    for (FloatLiteral *lit = float_literals; lit; lit = lit->next)
    {
        if (strcmp(lit->value, value) == 0)
        {
            return lit->label;
        }
    }
    fprintf(stderr, "[Codegen Error] Float literal '%s' not found\n", value);
    exit(1);
}

static void collect_float_literals(ASTNode *node)
{
    if (!node) return;

    switch (node->type)
    {
    case AST_NUMBER_LITERAL:
        if (node->result_type == TYPE_FLOAT)
        {
            add_float_literal(node->literal);
        }
        break;
    case AST_BINARY_EXPR:
        collect_float_literals(node->left);
        collect_float_literals(node->right);
        break;
    case AST_VAR_DECL:
        collect_float_literals(node->value);
        break;
    default:
        break;
    }

    if (node->next)
    {
        collect_float_literals(node->next);
    }
}

static void generate_expression(ASTNode *node, FILE *output, Symbol *symbols);
static void generate_data_section(ASTNode *program, FILE *output, Symbol **symbols);
static void generate_float_literals(FILE *output);

void generate_program(ASTNode *program, FILE *output)
{
    Symbol *symbols = NULL;

    collect_float_literals(program);

    fprintf(output, "    .intel_syntax noprefix\n");
    fprintf(output, "    .section .rodata\n");
    generate_float_literals(output);

    fprintf(output, "    .data\n");
    generate_data_section(program, output, &symbols);

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

            if (current->var_type == TYPE_FLOAT)
            {
                fprintf(output, "    movsd [rip + %s], xmm0\n", current->name);
            }
            else
            {
                fprintf(output, "    mov [rip + %s], rax\n", current->name);
            }

            last_var_decl = current;
        }
        current = current->next;
    }

    if (last_var_decl)
    {
        if (last_var_decl->var_type == TYPE_FLOAT)
        {
            fprintf(output, "    movsd xmm0, [rip + %s]\n", last_var_decl->name);
            fprintf(output, "    movq rax, xmm0\n");
        }
        else
        {
            fprintf(output, "    mov rax, [rip + %s]\n", last_var_decl->name);
        }
    }
    else
    {
        fprintf(output, "    mov rax, 0\n");
    }

    fprintf(output, "    ret\n");

    free_symbol_table(symbols);

    while (float_literals)
    {
        FloatLiteral *next = float_literals->next;
        free(float_literals->label);
        free(float_literals->value);
        free(float_literals);
        float_literals = next;
    }
}

static void generate_data_section(ASTNode *program, FILE *output, Symbol **symbols)
{
    ASTNode *current = program;
    while (current)
    {
        if (current->type == AST_VAR_DECL)
        {
            *symbols = add_symbol(*symbols, current->name, current->var_type);

            if (current->var_type == TYPE_FLOAT)
            {
                fprintf(output, "%s: .double 0.0\n", current->name);
            }
            else
            {
                fprintf(output, "%s: .quad 0\n", current->name);
            }
        }
        current = current->next;
    }
}

static void generate_float_literals(FILE *output)
{
    for (FloatLiteral *lit = float_literals; lit; lit = lit->next)
    {
        fprintf(output, "%s: .double %s\n", lit->label, lit->value);
    }
}

static void generate_expression(ASTNode *node, FILE *output, Symbol *symbols)
{
    if (!node) return;

    switch (node->type)
    {
    case AST_NUMBER_LITERAL:
        if (node->result_type == TYPE_FLOAT)
        {
            const char *label = get_float_label(node->literal);
            fprintf(output, "    movsd xmm0, [rip + %s]\n", label);
        }
        else
        {
            fprintf(output, "    mov rax, %s\n", node->literal);
        }
        break;

    case AST_IDENTIFIER:
    {
        Symbol *sym = lookup_symbol(symbols, node->name);
        if (!sym)
        {
            fprintf(stderr, "[Codegen Error] Undefined variable: %s\n", node->name);
            exit(1);
        }
        if (sym->type == TYPE_FLOAT)
        {
            fprintf(output, "    movsd xmm0, [rip + %s]\n", node->name);
        }
        else
        {
            fprintf(output, "    mov rax, [rip + %s]\n", node->name);
        }
        break;
    }

    case AST_BINARY_EXPR:
        generate_expression(node->right, output, symbols);

        if (node->result_type == TYPE_FLOAT)
        {
            fprintf(output, "    sub rsp, 8\n");
            fprintf(output, "    movsd [rsp], xmm0\n");
            generate_expression(node->left, output, symbols);
            fprintf(output, "    movsd xmm1, [rsp]\n");
            fprintf(output, "    add rsp, 8\n");

            switch (node->op)
            {
            case TOKEN_PLUS:
                fprintf(output, "    addsd xmm0, xmm1\n");
                break;
            case TOKEN_MINUS:
                fprintf(output, "    subsd xmm0, xmm1\n");
                break;
            case TOKEN_STAR:
                fprintf(output, "    mulsd xmm0, xmm1\n");
                break;
            case TOKEN_SLASH:
                fprintf(output, "    divsd xmm0, xmm1\n");
                break;
            default:
                fprintf(output, "    ; [unsupported binary op]\n");
            }
        }
        else
        {
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
        }
        break;

    default:
        fprintf(output, "    ; [unsupported node type]\n");
        break;
    }
}
