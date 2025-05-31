/**
 * @file ast.c
 * @brief Implementation of AST node creation and management functions for the SEG language compiler.
 *        Provides functions to create various types of AST nodes and to free the allocated memory.
 *        Ensures efficient memory management and supports the construction of the AST during parsing.
 * @author Dario Romandini
 */

#include <stdlib.h>
#include <string.h>
#include "ast.h"

static char *strdup_safe(const char *s)
{
    if (s == NULL)
        return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (copy)
    {
        strcpy(copy, s);
    }
    return copy;
}

ASTNode *create_var_decl_node(VarType var_type, const char *name, ASTNode *value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_VAR_DECL;
    node->result_type = var_type;
    node->next = NULL;
    node->var_decl.var_type = var_type;
    node->var_decl.name = strdup_safe(name);
    node->var_decl.value = value;
    return node;
}

ASTNode *create_literal_node(const char *value, VarType type)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_LITERAL;
    node->result_type = type;
    node->next = NULL;
    node->literal.value = strdup_safe(value);
    return node;
}

ASTNode *create_identifier_node(const char *name)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->result_type = TYPE_UNKNOWN;
    node->next = NULL;
    node->identifier.name = strdup_safe(name);
    return node;
}

ASTNode *create_binary_expr_node(TokenType op, ASTNode *left, ASTNode *right)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_BINARY_EXPR;
    node->result_type = TYPE_UNKNOWN;
    node->next = NULL;
    node->binary_expr.op = op;
    node->binary_expr.left = left;
    node->binary_expr.right = right;
    return node;
}

ASTNode *create_unary_expr_node(TokenType op, ASTNode *operand)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_UNARY_EXPR;
    node->result_type = TYPE_UNKNOWN;
    node->next = NULL;
    node->unary_expr.op = op;
    node->unary_expr.operand = operand;
    return node;
}

ASTNode *create_if_statement_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_IF_STATEMENT;
    node->result_type = TYPE_UNKNOWN;
    node->next = NULL;
    node->if_statement.condition = condition;
    node->if_statement.then_branch = then_branch;
    node->if_statement.else_branch = else_branch;
    return node;
}

void free_ast(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_VAR_DECL:
        free(node->var_decl.name);
        free_ast(node->var_decl.value);
        break;
    case AST_LITERAL:
        free(node->literal.value);
        break;
    case AST_IDENTIFIER:
        free(node->identifier.name);
        break;
    case AST_BINARY_EXPR:
        free_ast(node->binary_expr.left);
        free_ast(node->binary_expr.right);
        break;
    case AST_UNARY_EXPR:
        free_ast(node->unary_expr.operand);
        break;
    case AST_IF_STATEMENT:
        free_ast(node->if_statement.condition);
        free_ast(node->if_statement.then_branch);
        free_ast(node->if_statement.else_branch);
        break;
    default:
        break;
    }

    free_ast(node->next);
    free(node);
}
