/**
 * @file ast.h
 * @brief AST (Abstract Syntax Tree) structures and functions for the SEG language.
 *        Defines node types for variable declarations, expressions (binary and unary),
 *        literals, and variable references. Supports various literal types: int, float, bool, char, string.
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
    AST_UNARY_EXPR, /**< Unary expression (e.g., !x) */
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
    ASTNodeType type; /**< Type of the AST node */
    VarType result_type; /**< The evaluated type of the expression */
    VarType var_type; /**< The declared type for variable declarations */
    char *name; /**< Variable name for declarations and identifiers */
    struct ASTNode *value; /**< Assigned value for variable declarations */

    TokenType op; /**< Operator token type for binary or unary expressions */
    struct ASTNode *left; /**< Left child node (or operand for unary) */
    struct ASTNode *right; /**< Right child node (for binary expressions only) */

    char *literal; /**< String representation of literal values */

    struct ASTNode *next; /**< Next node in a linked list of statements */
} ASTNode;

/**
 * @brief Create a new variable declaration node.
 * @param var_type The declared type (e.g., TYPE_INT, TYPE_BOOL)
 * @param name The name of the variable
 * @param value The initial value as an ASTNode
 * @return Pointer to the created ASTNode
 */
ASTNode *create_var_decl_node(VarType var_type, const char *name, ASTNode *value);

/**
 * @brief Create a new binary expression node.
 * @param op The binary operator token (e.g., TOKEN_PLUS, TOKEN_AND)
 * @param left The left-hand side ASTNode
 * @param right The right-hand side ASTNode
 * @return Pointer to the created ASTNode
 */
ASTNode *create_binary_expr_node(TokenType op, ASTNode *left, ASTNode *right);

/**
 * @brief Create a new unary expression node (e.g., !x).
 * @param op The unary operator token (e.g., TOKEN_NOT)
 * @param operand The operand ASTNode
 * @return Pointer to the created ASTNode
 */
ASTNode *create_unary_expr_node(TokenType op, ASTNode *operand);

/**
 * @brief Create a new literal node (int, float, bool, char, string).
 * @param value The string representation of the literal
 * @param type The data type of the literal
 * @return Pointer to the created ASTNode
 */
ASTNode *create_literal_node(const char *value, VarType type);

/**
 * @brief Create a new identifier node.
 * @param name The name of the identifier
 * @return Pointer to the created ASTNode
 */
ASTNode *create_identifier_node(const char *name);

/**
 * @brief Free an entire AST linked list, including all subtrees.
 * @param node The root node of the AST to free
 */
void free_ast(ASTNode *node);

#endif // AST_H
