/**
* @file parser.h
 * @brief Parser interface for the SEG language compiler.
 *        Converts token streams into an Abstract Syntax Tree (AST) for variable declarations and expressions.
 *        Supports type checking and warnings for type promotions.
 *        Handles arithmetic and logical operators (AND, OR, XOR, NOT).
 * @author Dario Romandini
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include "lexer.h"
#include "ast.h"

/**
 * @brief Parser state structure.
 *
 * Holds the current token and lexer state for building the AST.
 */
typedef struct {
    Lexer *lexer; /**< Pointer to the associated lexer */
    Token current_token; /**< The current token being processed */
} Parser;

/**
 * @brief Initialize the parser.
 * @param parser Pointer to the parser state.
 * @param lexer Pointer to the lexer to use for token input.
 */
void parser_init(Parser *parser, Lexer *lexer);

/**
 * @brief Parse the entire SEG program into an AST.
 * @param parser Pointer to the parser state.
 * @return Pointer to the root of the AST.
 */
ASTNode *parse_program(Parser *parser);

/**
 * @brief Parse a single variable declaration.
 *        Expects a type keyword (int, float, bool, char, string) followed by an identifier and an assignment.
 * @param parser Pointer to the parser state.
 * @return Pointer to the AST node representing the variable declaration.
 */
ASTNode *parse_var_decl(Parser *parser);

/**
 * @brief Parse an expression, including arithmetic (+, -, *, /) and logical (&&, ||, ^, !) operators.
 * @param parser Pointer to the parser state.
 * @return Pointer to the AST node representing the expression.
 */
ASTNode *parse_expression(Parser *parser);

#endif // PARSER_H
