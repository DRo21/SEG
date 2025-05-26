/**
 * @file token.h
 * @brief Token definitions for the SEG language compiler.
 *        Defines token types and token structure used by the lexer and parser.
 *        This is the fundamental unit of lexical analysis in SEG, 
 *        representing keywords, identifiers, literals, and symbols in the source code.
 * @author Dario Romandini
 */

#ifndef TOKEN_H
#define TOKEN_H

/**
 * @brief Enumerated types for all possible SEG tokens.
 *
 * This includes keywords (e.g., int, float), identifiers, literals, operators, and punctuation.
 */
typedef enum {
    TOKEN_EOF,           ///< End of file marker
    TOKEN_INT,           ///< 'int' keyword
    TOKEN_FLOAT,         ///< 'float' keyword
    TOKEN_IDENTIFIER,    ///< Variable name or user-defined identifier
    TOKEN_NUMBER,        ///< Numeric literal (integer or float)

    // Operators
    TOKEN_ASSIGN,        ///< '=' assignment operator
    TOKEN_PLUS,          ///< '+' addition operator
    TOKEN_MINUS,         ///< '-' subtraction operator
    TOKEN_STAR,          ///< '*' multiplication operator
    TOKEN_SLASH,         ///< '/' division operator

    // Punctuation
    TOKEN_SEMICOLON,     ///< ';' statement terminator
    TOKEN_LPAREN,        ///< '(' left parenthesis
    TOKEN_RPAREN,        ///< ')' right parenthesis

    // Error handling
    TOKEN_ERROR          ///< Unknown or invalid token
} TokenType;

/**
 * @brief Token structure representing a single lexeme.
 *
 * Contains the type of the token, its textual value (lexeme), and the line number for error reporting.
 */
typedef struct {
    TokenType type;  ///< The type of token (from TokenType enum)
    char* lexeme;    ///< The string representation of the token (e.g., "int", "x", "5")
    int line;        ///< Line number where the token was found
} Token;

#endif // TOKEN_H
