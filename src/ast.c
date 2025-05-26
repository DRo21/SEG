/**
 * @file ast.c
 * @brief AST (Abstract Syntax Tree) node creation and memory management for SEG language.
 *        Provides functions to create and free AST nodes representing variable declarations,
 *        binary expressions, numeric literals, and identifiers.
 *        Supports expressions like: int z = 5 + 3;
 * @author Dario Romandini
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

ASTNode *create_var_decl_node(VarType var_type, const char *name, ASTNode *value)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Failed to allocate memory for AST node.\n");
        exit(1);
    }
    node->type = AST_VAR_DECL;
    node->var_type = var_type;
    node->name = strdup(name);
    node->value = value;
    node->op = 0;
    node->left = NULL;
    node->right = NULL;
    node->literal = NULL;
    node->next = NULL;
    return node;
}

ASTNode *create_binary_expr_node(TokenType op, ASTNode *left, ASTNode *right)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Failed to allocate memory for AST node.\n");
        exit(1);
    }
    node->type = AST_BINARY_EXPR;
    node->op = op;
    node->left = left;
    node->right = right;
    node->name = NULL;
    node->value = NULL;
    node->literal = NULL;
    node->next = NULL;
    return node;
}

ASTNode *create_number_literal_node(const char *literal)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Failed to allocate memory for AST node.\n");
        exit(1);
    }
    node->type = AST_NUMBER_LITERAL;
    node->literal = strdup(literal);
    node->name = NULL;
    node->value = NULL;
    node->op = 0;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    return node;
}

ASTNode *create_identifier_node(const char *name)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node)
    {
        fprintf(stderr, "Error: Failed to allocate memory for AST node.\n");
        exit(1);
    }
    node->type = AST_IDENTIFIER;
    node->name = strdup(name);
    node->literal = NULL;
    node->value = NULL;
    node->op = 0;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    return node;
}

void free_ast(ASTNode *node)
{
    while (node)
    {
        ASTNode *next = node->next;

        if (node->type == AST_VAR_DECL)
        {
            free(node->name);
            if (node->value)
            {
                free_ast(node->value);
            }
        }
        else if (node->type == AST_BINARY_EXPR)
        {
            if (node->left)
                free_ast(node->left);
            if (node->right)
                free_ast(node->right);
        }
        else if (node->type == AST_NUMBER_LITERAL)
        {
            free(node->literal);
        }
        else if (node->type == AST_IDENTIFIER)
        {
            free(node->name);
        }

        free(node);
        node = next;
    }
}
