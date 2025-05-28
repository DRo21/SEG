/**
 * @file lexer.c
 * @brief Lexer implementation for the SEG language compiler.
 *        Converts source code into a stream of tokens for parsing.
 *        Supports variable declarations, literals (int, float, bool, char, string), arithmetic operators, and parentheses.
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

TokenType check_keyword(const char *str) {
    if (strcmp(str, "int") == 0) return TOKEN_INT;
    if (strcmp(str, "float") == 0) return TOKEN_FLOAT;
    if (strcmp(str, "bool") == 0) return TOKEN_BOOL;
    if (strcmp(str, "char") == 0) return TOKEN_CHAR;
    if (strcmp(str, "string") == 0) return TOKEN_STRING;
    if (strcmp(str, "true") == 0 || strcmp(str, "false") == 0) return TOKEN_BOOL_LITERAL;
    return TOKEN_IDENTIFIER;
}

void lexer_init(Lexer *lexer, FILE *source) {
    lexer->source = source;
    lexer->line = 1;
}

Token lexer_next_token(Lexer *lexer) {
    int c;
    Token token = {0};
    token.line = lexer->line;

    while ((c = fgetc(lexer->source)) != EOF) {
        if (c == ' ' || c == '\t' || c == '\r') continue;
        if (c == '\n') {
            lexer->line++;
            continue;
        }
        break;
    }

    if (c == EOF) {
        token.type = TOKEN_EOF;
        return token;
    }

    if (isalpha(c) || c == '_') {
        char buffer[64] = {0};
        int i = 0;
        buffer[i++] = c;
        while ((c = fgetc(lexer->source)) != EOF && (isalnum(c) || c == '_')) buffer[i++] = c;
        ungetc(c, lexer->source);
        token.type = check_keyword(buffer);
        token.lexeme = strdup(buffer);
        return token;
    }

    if (isdigit(c)) {
        char buffer[64] = {0};
        int i = 0;
        buffer[i++] = c;
        while ((c = fgetc(lexer->source)) != EOF && (isdigit(c) || c == '.')) buffer[i++] = c;
        ungetc(c, lexer->source);
        token.type = TOKEN_NUMBER;
        token.lexeme = strdup(buffer);
        return token;
    }

    if (c == '\'') {
        char buffer[4] = {0};
        buffer[0] = fgetc(lexer->source);
        if (fgetc(lexer->source) != '\'') {
            token.type = TOKEN_ERROR;
            token.lexeme = strdup("Unterminated char");
            return token;
        }
        token.type = TOKEN_CHAR_LITERAL;
        token.lexeme = strdup(buffer);
        return token;
    }

    if (c == '"') {
        char buffer[256] = {0};
        int i = 0;
        while ((c = fgetc(lexer->source)) != EOF && c != '"') {
            if (c == '\n') {
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

    switch (c) {
        case '=': token.type = TOKEN_ASSIGN;
            break;
        case '+': token.type = TOKEN_PLUS;
            break;
        case '-': token.type = TOKEN_MINUS;
            break;
        case '*': token.type = TOKEN_STAR;
            break;
        case '/': token.type = TOKEN_SLASH;
            break;
        case ';': token.type = TOKEN_SEMICOLON;
            break;
        case '(': token.type = TOKEN_LPAREN;
            break;
        case ')': token.type = TOKEN_RPAREN;
            break;
        case '&': if ((c = fgetc(lexer->source)) == '&') {
                token.type = TOKEN_AND;
                token.lexeme[1] = '&';
                token.lexeme[2] = '\0';
            } else {
                ungetc(c, lexer->source);
                token.type = TOKEN_ERROR;
            }
            break;
        case '|': if ((c = fgetc(lexer->source)) == '|') {
                token.type = TOKEN_OR;
                token.lexeme[1] = '|';
                token.lexeme[2] = '\0';
            } else {
                ungetc(c, lexer->source);
                token.type = TOKEN_ERROR;
            }
            break;
        case '!': token.type = TOKEN_NOT;
            break;
        case '^': token.type = TOKEN_XOR;
            break;
        default: token.type = TOKEN_ERROR;
            break;
    }

    return token;
}

void token_free(Token *token) {
    if (token->lexeme) {
        free(token->lexeme);
        token->lexeme = NULL;
    }
}

const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_INT: return "INT";
        case TOKEN_FLOAT: return "FLOAT";
        case TOKEN_BOOL: return "BOOL";
        case TOKEN_CHAR: return "CHAR";
        case TOKEN_STRING: return "STRING";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_BOOL_LITERAL: return "BOOL_LITERAL";
        case TOKEN_CHAR_LITERAL: return "CHAR_LITERAL";
        case TOKEN_STRING_LITERAL: return "STRING_LITERAL";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_XOR: return "XOR";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}
