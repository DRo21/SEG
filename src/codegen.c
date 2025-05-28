/**
 * @file codegen.c
 * @brief Code generator for the SEG compiler. Translates AST to x86-64 assembly.
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "symbol.h"

static int literal_counter = 0;

typedef struct LiteralEntry {
    char *label;
    char *value;
    VarType type;
    struct LiteralEntry *next;
} LiteralEntry;

static LiteralEntry *literals = NULL;

static void add_literal(const char *value, VarType type) {
    for (LiteralEntry *lit = literals; lit; lit = lit->next) {
        if (strcmp(lit->value, value) == 0 && lit->type == type) return;
    }

    LiteralEntry *lit = (LiteralEntry *) malloc(sizeof(LiteralEntry));
    lit->label = (char *) malloc(32);
    sprintf(lit->label, "L_literal_%d", literal_counter++);
    lit->value = strdup(value);
    lit->type = type;
    lit->next = literals;
    literals = lit;
}

static const char *get_literal_label(const char *value, VarType type) {
    for (LiteralEntry *lit = literals; lit; lit = lit->next) {
        if (strcmp(lit->value, value) == 0 && lit->type == type) {
            return lit->label;
        }
    }
    fprintf(stderr, "[Codegen Error] Literal '%s' not found\n", value);
    exit(1);
}

static void collect_literals(ASTNode *node) {
    if (!node) return;

    if (node->type == AST_LITERAL) {
        if (node->result_type == TYPE_FLOAT || node->result_type == TYPE_BOOL ||
            node->result_type == TYPE_CHAR || node->result_type == TYPE_STRING) {
            add_literal(node->literal, node->result_type);
        }
    } else if (node->type == AST_BINARY_EXPR) {
        collect_literals(node->left);
        collect_literals(node->right);
    } else if (node->type == AST_VAR_DECL) {
        collect_literals(node->value);
    }

    if (node->next) {
        collect_literals(node->next);
    }
}

static void generate_expression(ASTNode *node, FILE *output, Symbol *symbols);

static void generate_data_section(ASTNode *program, FILE *output, Symbol **symbols);

static void generate_literals_section(FILE *output);

void generate_program(ASTNode *program, FILE *output) {
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
    ASTNode *last_var_decl = NULL;

    while (current) {
        if (current->type == AST_VAR_DECL) {
            generate_expression(current->value, output, symbols);

            if (current->var_type == TYPE_FLOAT) {
                fprintf(output, "    movsd [rip + %s], xmm0\n", current->name);
            } else {
                fprintf(output, "    mov [rip + %s], rax\n", current->name);
            }

            last_var_decl = current;
        }
        current = current->next;
    }

    if (last_var_decl) {
        if (last_var_decl->var_type == TYPE_FLOAT) {
            fprintf(output, "    movsd xmm0, [rip + %s]\n", last_var_decl->name);
            fprintf(output, "    movq rax, xmm0\n");
        } else {
            fprintf(output, "    mov rax, [rip + %s]\n", last_var_decl->name);
        }
    } else {
        fprintf(output, "    mov rax, 0\n");
    }

    fprintf(output, "    ret\n");

    free_symbol_table(symbols);

    while (literals) {
        LiteralEntry *next = literals->next;
        free(literals->label);
        free(literals->value);
        free(literals);
        literals = next;
    }
}

static void generate_data_section(ASTNode *program, FILE *output, Symbol **symbols) {
    ASTNode *current = program;
    while (current) {
        if (current->type == AST_VAR_DECL) {
            *symbols = add_symbol(*symbols, current->name, current->var_type);

            if (current->var_type == TYPE_FLOAT) {
                fprintf(output, "%s: .double 0.0\n", current->name);
            } else {
                fprintf(output, "%s: .quad 0\n", current->name);
            }
        }
        current = current->next;
    }
}

static void generate_literals_section(FILE *output) {
    for (LiteralEntry *lit = literals; lit; lit = lit->next) {
        switch (lit->type) {
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

static void generate_expression(ASTNode *node, FILE *output, Symbol *symbols) {
    if (!node) return;

    switch (node->type) {
        case AST_LITERAL:
            if (node->result_type == TYPE_FLOAT) {
                const char *label = get_literal_label(node->literal, TYPE_FLOAT);
                fprintf(output, "    movsd xmm0, [rip + %s]\n", label);
            } else if (node->result_type == TYPE_BOOL || node->result_type == TYPE_CHAR) {
                const char *label = get_literal_label(node->literal, node->result_type);
                fprintf(output, "    mov rax, [rip + %s]\n", label);
            } else if (node->result_type == TYPE_STRING) {
                const char *label = get_literal_label(node->literal, TYPE_STRING);
                fprintf(output, "    lea rax, [rip + %s]\n", label);
            } else {
                fprintf(output, "    mov rax, %s\n", node->literal);
            }
            break;

        case AST_IDENTIFIER: {
            Symbol *sym = lookup_symbol(symbols, node->name);
            if (!sym) {
                fprintf(stderr, "[Codegen Error] Undefined variable: %s\n", node->name);
                exit(1);
            }
            if (sym->type == TYPE_FLOAT) {
                fprintf(output, "    movsd xmm0, [rip + %s]\n", node->name);
            } else {
                fprintf(output, "    mov rax, [rip + %s]\n", node->name);
            }
            break;
        }

        case AST_BINARY_EXPR:
            generate_expression(node->right, output, symbols);
            fprintf(output, "    push rax\n");
            generate_expression(node->left, output, symbols);
            fprintf(output, "    pop rbx\n");

            switch (node->op) {
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
                case TOKEN_AND:
                    fprintf(output, "    and rax, rbx\n");
                    break;
                case TOKEN_OR:
                    fprintf(output, "    or rax, rbx\n");
                    break;
                case TOKEN_XOR:
                    fprintf(output, "    xor rax, rbx\n");
                    break;
                case TOKEN_EQ:
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    sete al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                case TOKEN_NEQ:
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setne al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                case TOKEN_LT:
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setl al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                case TOKEN_LEQ:
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setle al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                case TOKEN_GT:
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setg al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                case TOKEN_GEQ:
                    fprintf(output, "    cmp rax, rbx\n");
                    fprintf(output, "    setge al\n");
                    fprintf(output, "    movzx rax, al\n");
                    break;
                default:
                    fprintf(output, "    ; [unsupported binary op]\n");
                    break;
            }
            break;

        case AST_UNARY_EXPR:
            generate_expression(node->left, output, symbols);
            if (node->op == TOKEN_NOT) {
                fprintf(output, "    cmp rax, 0\n");
                fprintf(output, "    sete al\n");
                fprintf(output, "    movzx rax, al\n");
            } else {
                fprintf(output, "    ; [unsupported unary op]\n");
            }
            break;

        default:
            fprintf(output, "    ; [unsupported node type]\n");
            break;
    }
}
