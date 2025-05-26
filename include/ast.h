/**
 * @file ast.h
 * @brief AST (Abstract Syntax Tree) structures and functions for SEG language.
 *        Defines nodes for variable declarations and basic program structure.
 * @author Dario Romandini
 */

#ifndef AST_H
#define AST_H

#include "token.h"

/**
 * @brief Types of AST nodes
 *
 * For now, we only support variable declarations.
 */
typedef enum
{
    AST_VAR_DECL ///< Variable declaration node (e.g., int x = 5;)
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
 * Represents a single element of the program (currently only variable declarations).
 * Forms a linked list for the entire program.
 */
typedef struct ASTNode
{
    ASTNodeType type;     ///< Type of this AST node (e.g., AST_VAR_DECL)
    VarType var_type;     ///< Variable type (int, float)
    char *name;           ///< Variable name (e.g., "x")
    char *value;          ///< Literal value as a string (e.g., "5", "3.14")
    struct ASTNode *next; ///< Pointer to the next AST node (for linked list structure)
} ASTNode;

/**
 * @brief Create a new variable declaration AST node
 *
 * @param var_type Variable type (int, float)
 * @param name Variable name (string)
 * @param value Literal value (string)
 * @return Pointer to the created ASTNode
 */
ASTNode *create_var_decl_node(VarType var_type, const char *name, const char *value);

/**
 * @brief Free the entire AST linked list
 *
 * @param node Pointer to the first node of the AST to free
 */
void free_ast(ASTNode *node);

#endif // AST_H
