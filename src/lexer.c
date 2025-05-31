/**
 * @file lexer.c
 * @brief Lexer implementation for the SEG language compiler.
 *        Converts source code into tokens including keywords, literals, operators, and symbols.
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"
#include "token.h"

static TokenType check_keyword(const char *str)
{
    if (strcmp(str, "int") == 0)
        return TOKEN_INT;
    if (strcmp(str, "float") == 0)
        return TOKEN_FLOAT;
    if (strcmp(str, "bool") == 0)
        return TOKEN_BOOL;
    if (strcmp(str, "char") == 0)
        return TOKEN_CHAR;
    if (strcmp(str, "string") == 0)
        return TOKEN_STRING;
    if (strcmp(str, "if") == 0)
        return TOKEN_IF;
    if (strcmp(str, "else") == 0)
        return TOKEN_ELSE;
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0)
        return TOKEN_BOOL_LITERAL;
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

    if (isalpha(c) || c == '_')
    {
        char buffer[64] = {0};
        int i = 0;
        buffer[i++] = c;
        while ((c = fgetc(lexer->source)) != EOF && (isalnum(c) || c == '_'))
            buffer[i++] = c;
        ungetc(c, lexer->source);
        token.type = check_keyword(buffer);
        token.lexeme = strdup(buffer);
        return token;
    }

    if (isdigit(c))
    {
        char buffer[64] = {0};
        int i = 0;
        buffer[i++] = c;
        while ((c = fgetc(lexer->source)) != EOF && (isdigit(c) || c == '.'))
            buffer[i++] = c;
        ungetc(c, lexer->source);
        token.type = TOKEN_NUMBER;
        token.lexeme = strdup(buffer);
        return token;
    }

    if (c == '\'')
    {
        char buffer[4] = {0};
        buffer[0] = fgetc(lexer->source);
        if (fgetc(lexer->source) != '\'')
        {
            token.type = TOKEN_ERROR;
            token.lexeme = strdup("Unterminated char");
            return token;
        }
        token.type = TOKEN_CHAR_LITERAL;
        token.lexeme = strdup(buffer);
        return token;
    }

    if (c == '"')
    {
        char buffer[256] = {0};
        int i = 0;
        while ((c = fgetc(lexer->source)) != EOF && c != '"')
        {
            if (c == '\n')
            {
                token.type = TOKEN_ERROR;
                token.lexeme = strdup("Unterminated string");
                return token;
            }
            buffer[i++] = c;
        }
        token.type = TOKEN_STRING_LITERAL;
        token.lexeme = strdup(buffer);
        return token;
    }

    token.lexeme = malloc(3);
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';

    switch (c)
    {
    case '=':
        if ((c = fgetc(lexer->source)) == '=')
        {
            token.type = TOKEN_EQ;
            token.lexeme[1] = '=';
            token.lexeme[2] = '\0';
        }
        else
        {
            ungetc(c, lexer->source);
            token.type = TOKEN_ASSIGN;
        }
        break;
    case '!':
        if ((c = fgetc(lexer->source)) == '=')
        {
            token.type = TOKEN_NEQ;
            token.lexeme[1] = '=';
            token.lexeme[2] = '\0';
        }
        else
        {
            ungetc(c, lexer->source);
            token.type = TOKEN_NOT;
        }
        break;
    case '<':
        if ((c = fgetc(lexer->source)) == '=')
        {
            token.type = TOKEN_LEQ;
            token.lexeme[1] = '=';
            token.lexeme[2] = '\0';
        }
        else
        {
            ungetc(c, lexer->source);
            token.type = TOKEN_LT;
        }
        break;
    case '>':
        if ((c = fgetc(lexer->source)) == '=')
        {
            token.type = TOKEN_GEQ;
            token.lexeme[1] = '=';
            token.lexeme[2] = '\0';
        }
        else
        {
            ungetc(c, lexer->source);
            token.type = TOKEN_GT;
        }
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
    case '{':
        token.type = TOKEN_LBRACE;
        break;
    case '}':
        token.type = TOKEN_RBRACE;
        break;
    case '&':
        if ((c = fgetc(lexer->source)) == '&')
        {
            token.type = TOKEN_AND;
            token.lexeme[1] = '&';
            token.lexeme[2] = '\0';
        }
        else
        {
            ungetc(c, lexer->source);
            token.type = TOKEN_ERROR;
        }
        break;
    case '|':
        if ((c = fgetc(lexer->source)) == '|')
        {
            token.type = TOKEN_OR;
            token.lexeme[1] = '|';
            token.lexeme[2] = '\0';
        }
        else
        {
            ungetc(c, lexer->source);
            token.type = TOKEN_ERROR;
        }
        break;
    case '^':
        token.type = TOKEN_XOR;
        break;
    default:
        token.type = TOKEN_ERROR;
        break;
    }

    return token;
}
