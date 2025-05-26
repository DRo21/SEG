/**
 * @file parser.c
 * @brief Parser implementation for the SEG language compiler.
 *        Handles parsing of variable declarations and arithmetic expressions into an AST.
 *        Supports types: int, float.
 *        Grammar (simplified):
 *          program        → (var_decl)* EOF
 *          var_decl       → (int|float) IDENTIFIER '=' expression ';'
 *          expression     → term (('+'|'-') term)*
 *          term           → factor (('*'|'/') factor)*
 *          factor         → NUMBER | IDENTIFIER | '(' expression ')'
 *
 *        Example:
 *          int x = 5 + 3;
 *          float y = 1.5 * (x - 2);
 *
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Helper: Advance to the next token
void advance(Parser *parser)
{
    token_free(&parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
}

// Helper: Expect a specific token type, exit on mismatch
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

// Forward declarations
ASTNode *parse_expression(Parser *parser);
ASTNode *parse_term(Parser *parser);
ASTNode *parse_factor(Parser *parser);

void parser_init(Parser *parser, Lexer *lexer)
{
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
}

// Parse a variable declaration: (int|float) IDENTIFIER '=' expression ';'
ASTNode *parse_var_decl(Parser *parser)
{
    VarType var_type;

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
    advance(parser);

    expect(parser, TOKEN_IDENTIFIER);
    char *name = strdup(parser->current_token.lexeme);
    advance(parser);

    expect(parser, TOKEN_ASSIGN);
    advance(parser);

    ASTNode *value = parse_expression(parser);

    expect(parser, TOKEN_SEMICOLON);
    advance(parser);

    return create_var_decl_node(var_type, name, value);
}

// Parse an expression: term (('+'|'-') term)*
ASTNode *parse_expression(Parser *parser)
{
    ASTNode *node = parse_term(parser);

    while (parser->current_token.type == TOKEN_PLUS ||
           parser->current_token.type == TOKEN_MINUS)
    {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *right = parse_term(parser);
        node = create_binary_expr_node(op, node, right);
    }

    return node;
}

// Parse a term: factor (('*'|'/') factor)*
ASTNode *parse_term(Parser *parser)
{
    ASTNode *node = parse_factor(parser);

    while (parser->current_token.type == TOKEN_STAR ||
           parser->current_token.type == TOKEN_SLASH)
    {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *right = parse_factor(parser);
        node = create_binary_expr_node(op, node, right);
    }

    return node;
}

// Parse a factor: NUMBER | IDENTIFIER | '(' expression ')'
ASTNode *parse_factor(Parser *parser)
{
    ASTNode *node = NULL;

    if (parser->current_token.type == TOKEN_NUMBER)
    {
        node = create_number_literal_node(parser->current_token.lexeme);
        advance(parser);
    }
    else if (parser->current_token.type == TOKEN_IDENTIFIER)
    {
        node = create_identifier_node(parser->current_token.lexeme);
        advance(parser);
    }
    else if (parser->current_token.type == TOKEN_LPAREN)
    {
        advance(parser);
        node = parse_expression(parser);
        expect(parser, TOKEN_RPAREN);
        advance(parser);
    }
    else
    {
        printf("[Parser Error] Unexpected token: %s\n",
               token_type_to_string(parser->current_token.type));
        exit(1);
    }

    return node;
}

// Parse the entire program: (var_decl)* EOF
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
