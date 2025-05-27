/**
 * @file ast.h
 * @brief AST (Abstract Syntax Tree) structures and functions for SEG language.
 *        Defines nodes for variable declarations, expressions, and binary operations.
 *        Supports variable assignments with arithmetic expressions (e.g., int x = 5 + 3;).
 * @author Dario Romandini
 */

#ifndef AST_H
#define AST_H

#include "token.h"

/**
 * @brief Types of AST nodes
 *
 * AST represents the structure of SEG programs.
 */
typedef enum
{
    AST_VAR_DECL,       ///< Variable declaration node (e.g., int x = 5;)
    AST_BINARY_EXPR,    ///< Binary operation node (e.g., 5 + 3)
    AST_NUMBER_LITERAL, ///< Numeric literal (e.g., 5, 3.14)
    AST_IDENTIFIER      ///< Variable reference (e.g., x)
} ASTNodeType;

/**
 * @brief SEG variable types
 */
typedef enum
{
    TYPE_INT,  ///< Integer variable
    TYPE_FLOAT ///< Floating-point variable
} VarType;

/**
 * @brief AST node structure
 *
 * Represents a single node in the Abstract Syntax Tree.
 * Forms a linked list for multiple statements.
 */
typedef struct ASTNode
{
    ASTNodeType type;   ///< Type of this AST node
    VarType result_type;///< The type of this node's value (int/float) after type checking

    // For AST_VAR_DECL:
    VarType var_type;      ///< Variable type (int, float)
    char *name;            ///< Variable name (e.g., "x")
    struct ASTNode *value; ///< Expression assigned to the variable

    // For AST_BINARY_EXPR:
    TokenType op;          ///< Operator (e.g., TOKEN_PLUS, TOKEN_MINUS)
    struct ASTNode *left;  ///< Left operand
    struct ASTNode *right; ///< Right operand

    // For AST_NUMBER_LITERAL:
    char *literal; ///< String representation of the number

    // For AST_IDENTIFIER:
    // Use `name` field from AST_VAR_DECL

    // Linked list for multiple statements:
    struct ASTNode *next; ///< Pointer to the next AST node
} ASTNode;

/**
 * @brief Create a new variable declaration AST node
 *
 * @param var_type Variable type (int, float)
 * @param name Variable name
 * @param value Pointer to the AST expression node (value assigned to variable)
 * @return Pointer to the created ASTNode
 */
ASTNode *create_var_decl_node(VarType var_type, const char *name, ASTNode *value);

/**
 * @brief Create a binary expression AST node
 *
 * @param op Operator token type (e.g., TOKEN_PLUS, TOKEN_MINUS)
 * @param left Left operand expression
 * @param right Right operand expression
 * @return Pointer to the created ASTNode
 */
ASTNode *create_binary_expr_node(TokenType op, ASTNode *left, ASTNode *right);

/**
 * @brief Create a numeric literal AST node
 *
 * @param literal String representation of the number (e.g., "5", "3.14")
 * @return Pointer to the created ASTNode
 */
ASTNode *create_number_literal_node(const char *literal, VarType type);

/**
 * @brief Create an identifier AST node
 *
 * @param name Variable name (e.g., "x")
 * @return Pointer to the created ASTNode
 */
ASTNode *create_identifier_node(const char *name);

/**
 * @brief Free the entire AST linked list
 *
 * @param node Pointer to the first node of the AST to free
 */
void free_ast(ASTNode *node);

#endif // AST_H
