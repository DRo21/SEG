/**
 * @file token.h
 * @brief Token definitions for the SEG language compiler.
 *        Defines token types and token structure used by the lexer and parser.
 *        Includes keywords, identifiers, literals, operators, and control flow tokens.
 * @author Dario Romandini
 */

#ifndef TOKEN_H
#define TOKEN_H

/**
 * @brief Enumerated types for all possible SEG tokens.
 */
typedef enum
{
    TOKEN_EOF,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_BOOL,
    TOKEN_CHAR,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_BOOL_LITERAL,
    TOKEN_CHAR_LITERAL,
    TOKEN_STRING_LITERAL,

    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,

    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_XOR,

    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LEQ,
    TOKEN_GEQ,

    TOKEN_IF,
    TOKEN_ELSE,

    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,

    TOKEN_ERROR
} TokenType;

/**
 * @brief Token structure representing a single lexeme.
 */
typedef struct
{
    TokenType type; /**< Type of token */
    char *lexeme;   /**< Textual value of the token */
    int line;       /**< Line number for error reporting */
} Token;

/**
 * @brief Converts a TokenType to a human-readable string.
 * @param type The TokenType to convert.
 * @return A string representing the token type.
 */
const char *token_type_to_string(TokenType type);

/**
 * @brief Frees memory allocated by a token's lexeme.
 * @param token The token whose lexeme is to be freed.
 */
void token_free(Token *token);

#endif // TOKEN_H
