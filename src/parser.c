/**
 * @file parser.c
 * @brief Parser implementation for the SEG language compiler.
 *        Handles parsing of variable declarations into an AST.
 *        Supports types: int, float.
 *        Grammar (simplified):
 *          program        → (var_decl)* EOF
 *          var_decl       → (int|float) IDENTIFIER '=' NUMBER ';'
 *
 *        Example:
 *          int x = 5;
 *          float y = 3.14;
 *
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

/**
 * @brief Helper function: Advance to the next token
 *
 * Frees the previous token and fetches a new one from the lexer.
 *
 * @param parser Pointer to the parser state
 */
void advance(Parser *parser)
{
    token_free(&parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
}

/**
 * @brief Helper function: Expect a specific token type
 *
 * Exits the program with an error message if the current token doesn't match.
 *
 * @param parser Pointer to the parser state
 * @param type Expected token type
 */
void expect(Parser *parser, TokenType type)
{
    if (parser->current_token.type != type)
    {
        printf("[Parser Error] Expected %s, got %s (line %d)\n",
               token_type_to_string(type),
               token_type_to_string(parser->current_token.type),
               parser->current_token.line);
        exit(1);
    }
}

void parser_init(Parser *parser, Lexer *lexer)
{
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
}

ASTNode *parse_var_decl(Parser *parser)
{
    VarType var_type;

    // Match variable type
    if (parser->current_token.type == TOKEN_INT)
    {
        var_type = TYPE_INT;
    }
    else if (parser->current_token.type == TOKEN_FLOAT)
    {
        var_type = TYPE_FLOAT;
    }
    else
    {
        printf("[Parser Error] Expected type keyword (int/float), got %s\n",
               token_type_to_string(parser->current_token.type));
        exit(1);
    }
    advance(parser); // consume type

    // Match identifier
    expect(parser, TOKEN_IDENTIFIER);
    char *name = strdup(parser->current_token.lexeme);
    advance(parser);

    // Match '='
    expect(parser, TOKEN_ASSIGN);
    advance(parser);

    // Match number
    expect(parser, TOKEN_NUMBER);
    char *value = strdup(parser->current_token.lexeme);
    advance(parser);

    // Match ';'
    expect(parser, TOKEN_SEMICOLON);
    advance(parser);

    return create_var_decl_node(var_type, name, value);
}

ASTNode *parse_program(Parser *parser)
{
    ASTNode *head = NULL;
    ASTNode *current = NULL;

    while (parser->current_token.type != TOKEN_EOF)
    {
        ASTNode *node = parse_var_decl(parser);

        if (!head)
        {
            head = node;
        }
        else
        {
            current->next = node;
        }
        current = node;
    }

    return head;
}
