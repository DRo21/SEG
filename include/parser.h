/**
 * @file parser.h
 * @brief Parser interface for the SEG language compiler.
 *        Converts token streams into an Abstract Syntax Tree (AST) for variable declarations and expressions.
 *        Uses a recursive descent parsing approach for simplicity and clarity.
 * @author Dario Romandini
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "lexer.h"
#include "ast.h"

/**
 * @brief Parser state structure
 *
 * Holds the current token from the lexer and a pointer to the lexer itself.
 * Used to build the AST.
 */
typedef struct
{
    Lexer* lexer;        ///< Pointer to the associated lexer
    Token current_token; ///< Current token being parsed
} Parser;

/**
 * @brief Initialize the parser
 *
 * @param parser Pointer to the parser state
 * @param lexer Pointer to the lexer to use for token input
 */
void parser_init(Parser* parser, Lexer* lexer);

/**
 * @brief Parse the entire SEG program
 *
 * Parses a list of variable declarations from the source code.
 *
 * @param parser Pointer to the parser state
 * @return Pointer to the head of the AST linked list (or NULL if empty)
 */
ASTNode* parse_program(Parser* parser);

/**
 * @brief Parse a single variable declaration (e.g., int x = 5 + 3;)
 *
 * Expects the current token to be a type keyword (int/float).
 * Advances the parser state and returns a new AST node.
 *
 * @param parser Pointer to the parser state
 * @return Pointer to the created ASTNode for the variable declaration
 */
ASTNode* parse_var_decl(Parser* parser);

/**
 * @brief Parse an expression (supports +, -, *, /, and parentheses)
 *
 * Uses recursive descent parsing with operator precedence:
 *   - Multiplication/division bind tighter than addition/subtraction
 *   - Left-to-right associativity
 *
 * @param parser Pointer to the parser state
 * @return Pointer to the AST node representing the parsed expression
 */
ASTNode* parse_expression(Parser* parser);

#endif // PARSER_H
