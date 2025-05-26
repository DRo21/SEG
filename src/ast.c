/**
 * @file ast.c
 * @brief AST (Abstract Syntax Tree) node creation and memory management for SEG language.
 *        Provides functions to create and free AST nodes representing variable declarations.
 * @author Dario Romandini
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

ASTNode *create_var_decl_node(VarType var_type, const char *name, const char *value)
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
    node->value = strdup(value);
    node->next = NULL;
    return node;
}

void free_ast(ASTNode *node)
{
    while (node)
    {
        ASTNode *next = node->next;
        free(node->name);
        free(node->value);
        free(node);
        node = next;
    }
}
