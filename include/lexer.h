/**
 * @file lexer.h
 * @brief Lexer interface for the SEG language compiler.
 *        Provides functions to tokenize input source code.
 * @author Dario Romandini
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include "token.h"

/**
 * @brief Lexer state structure
 *
 * Holds the source file and current line number for error tracking.
 */
typedef struct
{
    FILE *source; ///< Input file pointer
    int line;     ///< Current line number (starts at 1)
} Lexer;

/**
 * @brief Initialize the lexer with a source file
 *
 * @param lexer Pointer to the lexer state
 * @param source Input file to tokenize
 */
void lexer_init(Lexer *lexer, FILE *source);

/**
 * @brief Get the next token from the input
 *
 * @param lexer Pointer to the lexer state
 * @return The next Token in the input stream
 */
Token lexer_next_token(Lexer *lexer);

/**
 * @brief Free memory allocated by a token
 *
 * @param token Pointer to the token to free
 */
void token_free(Token *token);

/**
 * @brief Get a string representation of a token type (for debugging)
 *
 * @param type The token type enum
 * @return Human-readable string for the token type
 */
const char *token_type_to_string(TokenType type);

#endif // LEXER_H
