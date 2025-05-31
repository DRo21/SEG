/**
 * @file ast.h
 * @brief Abstract Syntax Tree (AST) definitions for the SEG language compiler.
 *        Defines the structures and functions for representing and manipulating
 *        the AST, including variable declarations, expressions, and control flow constructs.
 * @author Dario Romandini
 */

#ifndef AST_H
#define AST_H

#include "type.h"
#include "token.h"
#include "symbol.h"

/**
 * @brief Enumerated types for AST node kinds.
 */
typedef enum
{
    AST_VAR_DECL,    ///< Variable declaration
    AST_LITERAL,     ///< Literal value
    AST_IDENTIFIER,  ///< Identifier
    AST_BINARY_EXPR, ///< Binary expression
    AST_UNARY_EXPR,  ///< Unary expression
    AST_IF_STATEMENT ///< If statement
} ASTNodeType;

/**
 * @brief Structure representing an AST node.
 */
typedef struct ASTNode
{
    ASTNodeType type;     ///< Type of the AST node
    VarType result_type;  ///< Resulting type after evaluation
    struct ASTNode *next; ///< Pointer to the next AST node (for sequences)

    union
    {
        struct
        {
            VarType var_type;      ///< Type of the variable
            char *name;            ///< Name of the variable
            struct ASTNode *value; ///< Initial value assigned
        } var_decl;

        struct
        {
            char *value; ///< Literal value as a string
        } literal;

        struct
        {
            char *name; ///< Identifier name
        } identifier;

        struct
        {
            TokenType op;          ///< Operator token
            struct ASTNode *left;  ///< Left operand
            struct ASTNode *right; ///< Right operand
        } binary_expr;

        struct
        {
            TokenType op;            ///< Operator token
            struct ASTNode *operand; ///< Operand
        } unary_expr;

        struct
        {
            struct ASTNode *condition;   ///< Condition expression
            struct ASTNode *then_branch; ///< Then branch block
            struct ASTNode *else_branch; ///< Else branch block
        } if_statement;
    };
} ASTNode;

/**
 * @brief Creates a variable declaration AST node.
 * @param var_type The type of the variable.
 * @param name The name of the variable.
 * @param value The initial value assigned to the variable.
 * @return Pointer to the created ASTNode.
 */
ASTNode *create_var_decl_node(VarType var_type, const char *name, ASTNode *value);

/**
 * @brief Creates a literal AST node.
 * @param value The literal value as a string.
 * @param type The type of the literal.
 * @return Pointer to the created ASTNode.
 */
ASTNode *create_literal_node(const char *value, VarType type);

/**
 * @brief Creates an identifier AST node.
 * @param name The name of the identifier.
 * @return Pointer to the created ASTNode.
 */
ASTNode *create_identifier_node(const char *name);

/**
 * @brief Creates a binary expression AST node.
 * @param op The operator token.
 * @param left The left operand.
 * @param right The right operand.
 * @return Pointer to the created ASTNode.
 */
ASTNode *create_binary_expr_node(TokenType op, ASTNode *left, ASTNode *right);

/**
 * @brief
 * @param op The operator token.
 * @param operand The operand.
 * @return Pointer to the created ASTNode.
 */
ASTNode *create_unary_expr_node(TokenType op, ASTNode *operand);

/**
 * @brief Creates an if statement AST node.
 * @param condition The condition expression.
 * @param then_branch The then branch block.
 * @param else_branch The else branch block.
 * @return Pointer to the created ASTNode.
 */
ASTNode *create_if_statement_node(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);

/**
 * @brief Frees the memory allocated for an AST node and its children.
 * @param node Pointer to the ASTNode to be freed.
 */
void free_ast(ASTNode *node);

#endif // AST_H
