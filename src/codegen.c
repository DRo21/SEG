/**
 * @file codegen.c
 * @brief Code generator implementation for the SEG compiler.
 *        Translates AST into x86-64 assembly, handling literals, variables, expressions, and control flow.
 *        Supports printf for runtime output and type-safe code generation for int, float, bool, char, and string types.
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symbol.h"
#include "token.h" // For token_type_to_string()

static int literal_counter = 0;

typedef struct LiteralEntry
{
    char *label;
    char *value;
    VarType type;
    struct LiteralEntry *next;
} LiteralEntry;

static LiteralEntry *literals = NULL;

static void add_literal(const char *value, VarType type)
{
    for (LiteralEntry *lit = literals; lit; lit = lit->next)
    {
        if (strcmp(lit->value, value) == 0 && lit->type == type)
            return;
    }
    LiteralEntry *lit = malloc(sizeof(LiteralEntry));
    lit->label = malloc(32);
    sprintf(lit->label, "L_literal_%d", literal_counter++);
    lit->value = strdup(value);
    lit->type = type;
    lit->next = literals;
    literals = lit;
}

static const char *get_literal_label(const char *value, VarType type)
{
    for (LiteralEntry *lit = literals; lit; lit = lit->next)
    {
        if (strcmp(lit->value, value) == 0 && lit->type == type)
        {
            return lit->label;
        }
    }
    fprintf(stderr, "[Codegen Error] Literal '%s' not found\n", value);
    exit(1);
}

static void collect_literals(ASTNode *node)
{
    if (!node)
        return;
    switch (node->type)
    {
    case AST_LITERAL:
        if (node->result_type == TYPE_FLOAT || node->result_type == TYPE_BOOL ||
            node->result_type == TYPE_CHAR || node->result_type == TYPE_STRING)
        {
            add_literal(node->literal.value, node->result_type);
        }
        break;
    case AST_BINARY_EXPR:
        collect_literals(node->binary_expr.left);
        collect_literals(node->binary_expr.right);
        break;
    case AST_UNARY_EXPR:
        collect_literals(node->unary_expr.operand);
        break;
    case AST_VAR_DECL:
        collect_literals(node->var_decl.value);
        break;
    case AST_IF_STATEMENT:
        collect_literals(node->if_statement.condition);
        collect_literals(node->if_statement.then_branch);
        collect_literals(node->if_statement.else_branch);
        break;
    default:
        break;
    }
    collect_literals(node->next);
}

static void generate_expression(ASTNode *node, FILE *output, Symbol *symbols);
static void generate_data_section(ASTNode *program, FILE *output, Symbol **symbols);
static void generate_literals_section(FILE *output);

void generate_program(ASTNode *program, FILE *output)
{
    Symbol *symbols = NULL;

    collect_literals(program);

    fprintf(output, "    .intel_syntax noprefix\n");
    fprintf(output, "    .section .rodata\n");
    generate_literals_section(output);

    fprintf(output, "    .data\n");
    generate_data_section(program, output, &symbols);

    fprintf(output, "    .text\n");
    fprintf(output, "    .global main\n");
    fprintf(output, "main:\n");

    ASTNode *current = program;
    while (current)
    {
        if (current->type == AST_VAR_DECL)
        {
            generate_expression(current->var_decl.value, output, symbols);
            if (current->var_decl.var_type == TYPE_FLOAT)
            {
                fprintf(output, "    movsd [rip + %s], xmm0\n", current->var_decl.name);
            }
            else
            {
                fprintf(output, "    mov [rip + %s], rax\n", current->var_decl.name);
            }
        }
        else if (current->type == AST_IF_STATEMENT)
        {
            static int if_counter = 0;
            int label_num = if_counter++;
            char label_true[32], label_end[32], label_else[32];
            sprintf(label_true, "L_if_true_%d", label_num);
            sprintf(label_end, "L_if_end_%d", label_num);
            sprintf(label_else, "L_if_else_%d", label_num);

            generate_expression(current->if_statement.condition, output, symbols);
            fprintf(output, "    cmp rax, 0\n");
            fprintf(output, "    je %s\n", current->if_statement.else_branch ? label_else : label_end);
            fprintf(output, "%s:\n", label_true);
            generate_program(current->if_statement.then_branch, output);
            fprintf(output, "    jmp %s\n", label_end);
            if (current->if_statement.else_branch)
            {
                fprintf(output, "%s:\n", label_else);
                generate_program(current->if_statement.else_branch, output);
            }
            fprintf(output, "%s:\n", label_end);
        }
        current = current->next;
    }

    fprintf(output, "    mov rax, 0\n");
    fprintf(output, "    ret\n");

    free_symbol_table(symbols);

    while (literals)
    {
        LiteralEntry *next = literals->next;
        free(literals->label);
        free(literals->value);
        free(literals);
        literals = next;
    }
}

static void generate_data_section(ASTNode *program, FILE *output, Symbol **symbols)
{
    ASTNode *current = program;
    while (current)
    {
        if (current->type == AST_VAR_DECL)
        {
            *symbols = add_symbol(*symbols, current->var_decl.name, current->var_decl.var_type);
            if (current->var_decl.var_type == TYPE_FLOAT)
            {
                fprintf(output, "%s: .double 0.0\n", current->var_decl.name);
            }
            else
            {
                fprintf(output, "%s: .quad 0\n", current->var_decl.name);
            }
        }
        current = current->next;
    }
}

static void generate_literals_section(FILE *output)
{
    for (LiteralEntry *lit = literals; lit; lit = lit->next)
    {
        switch (lit->type)
        {
        case TYPE_FLOAT:
            fprintf(output, "%s: .double %s\n", lit->label, lit->value);
            break;
        case TYPE_BOOL:
            fprintf(output, "%s: .quad %s\n", lit->label, strcmp(lit->value, "true") == 0 ? "1" : "0");
            break;
        case TYPE_CHAR:
            fprintf(output, "%s: .byte '%s'\n", lit->label, lit->value);
            break;
        case TYPE_STRING:
            fprintf(output, "%s: .string \"%s\"\n", lit->label, lit->value);
            break;
        default:
            break;
        }
    }
}

static void generate_expression(ASTNode *node, FILE *output, Symbol *symbols)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_LITERAL:
    {
        const char *label = get_literal_label(node->literal.value, node->result_type);
        if (node->result_type == TYPE_FLOAT)
        {
            fprintf(output, "    movsd xmm0, [rip + %s]\n", label);
        }
        else if (node->result_type == TYPE_BOOL || node->result_type == TYPE_CHAR)
        {
            fprintf(output, "    mov rax, [rip + %s]\n", label);
        }
        else if (node->result_type == TYPE_STRING)
        {
            fprintf(output, "    lea rax, [rip + %s]\n", label);
        }
        else
        {
            fprintf(output, "    mov rax, %s\n", node->literal.value);
        }
        break;
    }
    case AST_IDENTIFIER:
    {
        Symbol *sym = lookup_symbol(symbols, node->identifier.name);
        if (!sym)
        {
            fprintf(stderr, "[Codegen Error] Undefined variable: %s\n", node->identifier.name);
            exit(1);
        }
        if (sym->type == TYPE_FLOAT)
        {
            fprintf(output, "    movsd xmm0, [rip + %s]\n", node->identifier.name);
        }
        else
        {
            fprintf(output, "    mov rax, [rip + %s]\n", node->identifier.name);
        }
        break;
    }
    case AST_BINARY_EXPR:
        generate_expression(node->binary_expr.right, output, symbols);
        fprintf(output, "    push rax\n");
        generate_expression(node->binary_expr.left, output, symbols);
        fprintf(output, "    pop rbx\n");
        switch (node->binary_expr.op)
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
            fprintf(output, "    cqo\n    idiv rbx\n");
            break;
        case TOKEN_EQ:
            fprintf(output, "    cmp rax, rbx\n    sete al\n    movzx rax, al\n");
            break;
        case TOKEN_NEQ:
            fprintf(output, "    cmp rax, rbx\n    setne al\n    movzx rax, al\n");
            break;
        case TOKEN_LT:
            fprintf(output, "    cmp rax, rbx\n    setl al\n    movzx rax, al\n");
            break;
        case TOKEN_LEQ:
            fprintf(output, "    cmp rax, rbx\n    setle al\n    movzx rax, al\n");
            break;
        case TOKEN_GT:
            fprintf(output, "    cmp rax, rbx\n    setg al\n    movzx rax, al\n");
            break;
        case TOKEN_GEQ:
            fprintf(output, "    cmp rax, rbx\n    setge al\n    movzx rax, al\n");
            break;
        case TOKEN_AND:
            fprintf(output, "    and rax, rbx\n");
            break;
        case TOKEN_OR:
            fprintf(output, "    or rax, rbx\n");
            break;
        case TOKEN_XOR:
            fprintf(output, "    xor rax, rbx\n");
            break;
        default:
            fprintf(output, "    ; [unsupported binary op]\n");
            break;
        }
        break;
    case AST_UNARY_EXPR:
        generate_expression(node->unary_expr.operand, output, symbols);
        if (node->unary_expr.op == TOKEN_NOT)
        {
            fprintf(output, "    cmp rax, 0\n");
            fprintf(output, "    sete al\n");
            fprintf(output, "    movzx rax, al\n");
        }
        break;
    default:
        fprintf(output, "    ; [unsupported node type]\n");
        break;
    }
}
