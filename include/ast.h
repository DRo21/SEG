/**
* @file ast.h
 * @brief AST (Abstract Syntax Tree) structures and functions for the SEG language.
 *        Defines node types for variable declarations, expressions, literals, and binary operations.
 *        Supports various literal types: int, float, bool, char, string.
 * @author Dario Romandini
 */

#ifndef AST_H
#define AST_H

#include "token.h"

/**
 * @brief Types of AST nodes.
 */
typedef enum {
    AST_VAR_DECL, /**< Variable declaration (e.g., int x = 5;) */
    AST_BINARY_EXPR, /**< Binary expression (e.g., x + 3) */
    AST_LITERAL, /**< Literal node (int, float, bool, char, string) */
    AST_IDENTIFIER /**< Variable reference (e.g., x) */
} ASTNodeType;

/**
 * @brief Supported data types in SEG.
 */
typedef enum {
    TYPE_INT, /**< Integer */
    TYPE_FLOAT, /**< Floating-point */
    TYPE_BOOL, /**< Boolean */
    TYPE_CHAR, /**< Character */
    TYPE_STRING /**< String */
} VarType;

/**
 * @brief AST node structure.
 */
typedef struct ASTNode {
    ASTNodeType type;
    VarType result_type;
    VarType var_type;
    char *name;
    struct ASTNode *value;

    TokenType op;
    struct ASTNode *left;
    struct ASTNode *right;

    char *literal;

    struct ASTNode *next;
} ASTNode;

/**
 * @brief Create a new variable declaration node.
 */
ASTNode *create_var_decl_node(VarType var_type, const char *name, ASTNode *value);

/**
 * @brief Create a new binary expression node.
 */
ASTNode *create_binary_expr_node(TokenType op, ASTNode *left, ASTNode *right);

/**
 * @brief Create a new literal node (int, float, bool, char, string).
 */
ASTNode *create_literal_node(const char *value, VarType type);

/**
 * @brief Create a new identifier node.
 */
ASTNode *create_identifier_node(const char *name);

/**
 * @brief Free an entire AST linked list, including all subtrees.
 */
void free_ast(ASTNode *node);

#endif // AST_H
