/**
 * @file parser.h
 * @brief Parser interface for the SEG language compiler.
 *        Converts token streams into an Abstract Syntax Tree (AST) for variable declarations, expressions, and control flow.
 *        Supports type checking and warnings for type promotions in arithmetic and logical expressions.
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
typedef struct
{
    Lexer *lexer;        /**< Pointer to the associated lexer */
    Token current_token; /**< The current token being processed */
} Parser;

/**
 * @brief Initializes the parser.
 * @param parser Pointer to the parser state.
 * @param lexer Pointer to the lexer to use for token input.
 */
void parser_init(Parser *parser, Lexer *lexer);

/**
 * @brief Parses the entire SEG program into an AST.
 * @param parser Pointer to the parser state.
 * @return Pointer to the root of the AST.
 */
ASTNode *parse_program(Parser *parser);

/**
 * @brief Parses a single variable declaration.
 *        Expects a type keyword (int, float, bool, char, string) followed by an identifier and an assignment.
 * @param parser Pointer to the parser state.
 * @return Pointer to the AST node representing the variable declaration.
 */
ASTNode *parse_var_decl(Parser *parser);

/**
 * @brief Parses an if-statement (with optional else or else-if branches).
 * @param parser Pointer to the parser state.
 * @return Pointer to the AST node representing the if-statement.
 */
ASTNode *parse_if_statement(Parser *parser);

/**
 * @brief Parses a single statement (variable declaration or if-statement).
 * @param parser Pointer to the parser state.
 * @return Pointer to the AST node representing the statement.
 */
ASTNode *parse_statement(Parser *parser);

/**
 * @brief Parses a block of statements (inside braces).
 * @param parser Pointer to the parser state.
 * @return Pointer to the root of the AST block.
 */
ASTNode *parse_block(Parser *parser);

/**
 * @brief Parses an expression, including arithmetic, logical, and comparison operators.
 * @param parser Pointer to the parser state.
 * @return Pointer to the AST node representing the expression.
 */
ASTNode *parse_expression(Parser *parser);

#endif // PARSER_H
