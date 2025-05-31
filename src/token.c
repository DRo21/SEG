/**
 * @file token.c
 * @brief Token utility functions for the SEG language compiler.
 *        Includes token-to-string conversion and token memory cleanup.
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include "token.h"

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
    case TOKEN_BOOL:
        return "BOOL";
    case TOKEN_CHAR:
        return "CHAR";
    case TOKEN_STRING:
        return "STRING";
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_NUMBER:
        return "NUMBER";
    case TOKEN_BOOL_LITERAL:
        return "BOOL_LITERAL";
    case TOKEN_CHAR_LITERAL:
        return "CHAR_LITERAL";
    case TOKEN_STRING_LITERAL:
        return "STRING_LITERAL";
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
    case TOKEN_AND:
        return "AND";
    case TOKEN_OR:
        return "OR";
    case TOKEN_NOT:
        return "NOT";
    case TOKEN_XOR:
        return "XOR";
    case TOKEN_EQ:
        return "EQ";
    case TOKEN_NEQ:
        return "NEQ";
    case TOKEN_LT:
        return "LT";
    case TOKEN_GT:
        return "GT";
    case TOKEN_LEQ:
        return "LEQ";
    case TOKEN_GEQ:
        return "GEQ";
    case TOKEN_IF:
        return "IF";
    case TOKEN_ELSE:
        return "ELSE";
    case TOKEN_SEMICOLON:
        return "SEMICOLON";
    case TOKEN_LPAREN:
        return "LPAREN";
    case TOKEN_RPAREN:
        return "RPAREN";
    case TOKEN_LBRACE:
        return "LBRACE";
    case TOKEN_RBRACE:
        return "RBRACE";
    case TOKEN_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

void token_free(Token *token)
{
    if (token->lexeme)
    {
        free(token->lexeme);
        token->lexeme = NULL;
    }
}
