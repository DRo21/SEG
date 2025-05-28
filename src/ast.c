/**
 * @file ast.c
 * @brief AST node creation and memory management for the SEG language compiler.
 * @author Dario Romandini
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

ASTNode *create_var_decl_node(VarType var_type, const char *name, ASTNode *value) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(1);
    }
    node->type = AST_VAR_DECL;
    node->var_type = var_type;
    node->result_type = var_type;
    node->name = strdup(name);
    node->value = value;
    node->op = 0;
    node->left = node->right = NULL;
    node->literal = NULL;
    node->next = NULL;
    return node;
}

ASTNode *create_binary_expr_node(TokenType op, ASTNode *left, ASTNode *right) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(1);
    }
    node->type = AST_BINARY_EXPR;
    node->op = op;
    node->left = left;
    node->right = right;
    node->result_type = TYPE_INT;
    node->name = node->literal = NULL;
    node->value = NULL;
    node->next = NULL;
    return node;
}

ASTNode *create_unary_expr_node(TokenType op, ASTNode *operand) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(1);
    }
    node->type = AST_UNARY_EXPR;
    node->op = op;
    node->left = operand;
    node->right = NULL;
    node->result_type = TYPE_BOOL;
    node->name = node->literal = NULL;
    node->value = NULL;
    node->next = NULL;
    return node;
}

ASTNode *create_literal_node(const char *value, VarType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(1);
    }
    node->type = AST_LITERAL;
    node->literal = strdup(value);
    node->result_type = type;
    node->name = NULL;
    node->value = NULL;
    node->op = 0;
    node->left = node->right = NULL;
    node->next = NULL;
    return node;
}

ASTNode *create_identifier_node(const char *name) {
    ASTNode *node = malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Out of memory\n");
        exit(1);
    }
    node->type = AST_IDENTIFIER;
    node->name = strdup(name);
    node->result_type = TYPE_INT; // Placeholder
    node->literal = NULL;
    node->value = NULL;
    node->op = 0;
    node->left = node->right = NULL;
    node->next = NULL;
    return node;
}

void free_ast(ASTNode *node) {
    while (node) {
        ASTNode *next = node->next;
        if (node->type == AST_VAR_DECL) {
            free(node->name);
            if (node->value) free_ast(node->value);
        } else if (node->type == AST_BINARY_EXPR) {
            if (node->left) free_ast(node->left);
            if (node->right) free_ast(node->right);
        } else if (node->type == AST_LITERAL || node->type == AST_IDENTIFIER) {
            if (node->literal) free(node->literal);
            if (node->name) free(node->name);
        }
        free(node);
        node = next;
    }
}
