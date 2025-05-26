/**
 * @file lexer.c
 * @brief Lexer implementation for the SEG language compiler.
 *        Converts source code into a stream of tokens for parsing.
 *        Supports variable declarations, arithmetic operators, and parentheses for expressions.
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

/**
 * @brief Check if a string matches a SEG keyword (int, float)
 *
 * @param str The string to check
 * @return The corresponding TokenType or TOKEN_IDENTIFIER if not a keyword
 */
TokenType check_keyword(const char *str)
{
    if (strcmp(str, "int") == 0)
        return TOKEN_INT;
    if (strcmp(str, "float") == 0)
        return TOKEN_FLOAT;
    return TOKEN_IDENTIFIER;
}

void lexer_init(Lexer *lexer, FILE *source)
{
    lexer->source = source;
    lexer->line = 1;
}

Token lexer_next_token(Lexer *lexer)
{
    int c;
    Token token = {0};
    token.line = lexer->line;

    // Skip whitespace and track newlines
    while ((c = fgetc(lexer->source)) != EOF)
    {
        if (c == ' ' || c == '\t' || c == '\r')
            continue;
        if (c == '\n')
        {
            lexer->line++;
            continue;
        }
        break;
    }

    if (c == EOF)
    {
        token.type = TOKEN_EOF;
        return token;
    }

    // Identifiers or keywords (int, float, variable names)
    if (isalpha(c) || c == '_')
    {
        char buffer[64] = {0};
        int i = 0;
        buffer[i++] = c;

        while ((c = fgetc(lexer->source)) != EOF && (isalnum(c) || c == '_'))
        {
            buffer[i++] = c;
        }
        ungetc(c, lexer->source);

        token.type = check_keyword(buffer);
        token.lexeme = strdup(buffer);
        return token;
    }

    // Numeric literals
    if (isdigit(c))
    {
        char buffer[64] = {0};
        int i = 0;
        buffer[i++] = c;

        while ((c = fgetc(lexer->source)) != EOF && (isdigit(c) || c == '.'))
        {
            buffer[i++] = c;
        }
        ungetc(c, lexer->source);

        token.type = TOKEN_NUMBER;
        token.lexeme = strdup(buffer);
        return token;
    }

    // Single-character tokens (operators, punctuation, parentheses)
    token.lexeme = malloc(2);
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';

    switch (c)
    {
    case '=':
        token.type = TOKEN_ASSIGN;
        break;
    case '+':
        token.type = TOKEN_PLUS;
        break;
    case '-':
        token.type = TOKEN_MINUS;
        break;
    case '*':
        token.type = TOKEN_STAR;
        break;
    case '/':
        token.type = TOKEN_SLASH;
        break;
    case ';':
        token.type = TOKEN_SEMICOLON;
        break;
    case '(':
        token.type = TOKEN_LPAREN;
        break;
    case ')':
        token.type = TOKEN_RPAREN;
        break;
    default:
        token.type = TOKEN_ERROR;
        break;
    }

    return token;
}

void token_free(Token *token)
{
    if (token->lexeme)
    {
        free(token->lexeme);
        token->lexeme = NULL;
    }
}

const char *token_type_to_string(TokenType type)
{
    switch (type)
    {
    case TOKEN_EOF:
        return "EOF";
    case TOKEN_INT:
        return "INT";
    case TOKEN_FLOAT:
        return "FLOAT";
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_NUMBER:
        return "NUMBER";
    case TOKEN_ASSIGN:
        return "ASSIGN";
    case TOKEN_PLUS:
        return "PLUS";
    case TOKEN_MINUS:
        return "MINUS";
    case TOKEN_STAR:
        return "STAR";
    case TOKEN_SLASH:
        return "SLASH";
    case TOKEN_SEMICOLON:
        return "SEMICOLON";
    case TOKEN_LPAREN:
        return "LPAREN";
    case TOKEN_RPAREN:
        return "RPAREN";
    case TOKEN_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}
