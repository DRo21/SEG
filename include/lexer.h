/**
 * @file lexer.h
 * @brief Lexer interface for the SEG language compiler.
 *        Converts source code into a stream of tokens for parsing.
 *        Supports variable declarations, literals, operators, control flow, and punctuation.
 * @author Dario Romandini
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"

/**
 * @brief Lexer state structure.
 * Holds the source file and current line number for error reporting.
 */
typedef struct
{
    FILE *source; /**< Input file pointer */
    int line;     /**< Current line number (starts at 1) */
} Lexer;

/**
 * @brief Initializes the lexer.
 * @param lexer Pointer to the lexer state.
 * @param source Input file to tokenize.
 */
void lexer_init(Lexer *lexer, FILE *source);

/**
 * @brief Gets the next token from the input.
 * @param lexer Pointer to the lexer state.
 * @return The next Token in the input stream.
 */
Token lexer_next_token(Lexer *lexer);

#endif // LEXER_H
