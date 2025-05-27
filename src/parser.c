/**
 * @file parser.c
 * @brief Parser implementation for the SEG language compiler.
 *        Handles parsing of variable declarations and arithmetic expressions into an AST.
 *        Supports types: int, float, and type promotion warnings.
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "symbol.h" // For type lookup (later if needed)

// Helpers
void advance(Parser *parser)
{
    token_free(&parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
}

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

    // Type checking: Warn if var type and expr type mismatch
    if (value->result_type != var_type)
    {
        printf("[Parser Warning] Type mismatch in assignment to '%s': declared %s, assigned %s (line %d). Implicit conversion applied.\n",
               name,
               var_type == TYPE_INT ? "int" : "float",
               value->result_type == TYPE_INT ? "int" : "float",
               parser->current_token.line);
    }

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

        // Type promotion
        if (node->result_type != right->result_type)
        {
            printf("[Parser Warning] Mixing int and float in expression: promoting int to float (line %d)\n",
                   parser->current_token.line);
            node->result_type = TYPE_FLOAT;
            right->result_type = TYPE_FLOAT;
        }
        else
        {
            node->result_type = node->result_type;
        }

        node = create_binary_expr_node(op, node, right);
        node->result_type = right->result_type; // Propagate type
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

        // Type promotion
        if (node->result_type != right->result_type)
        {
            printf("[Parser Warning] Mixing int and float in expression: promoting int to float (line %d)\n",
                   parser->current_token.line);
            node->result_type = TYPE_FLOAT;
            right->result_type = TYPE_FLOAT;
        }
        else
        {
            node->result_type = node->result_type;
        }

        node = create_binary_expr_node(op, node, right);
        node->result_type = right->result_type;
    }

    return node;
}

// Parse a factor: NUMBER | IDENTIFIER | '(' expression ')'
ASTNode *parse_factor(Parser *parser)
{
    ASTNode *node = NULL;

    if (parser->current_token.type == TOKEN_NUMBER)
    {
        // Detect if it's a float by presence of '.' in lexeme
        if (strchr(parser->current_token.lexeme, '.'))
            node = create_number_literal_node(parser->current_token.lexeme, TYPE_FLOAT);
        else
            node = create_number_literal_node(parser->current_token.lexeme, TYPE_INT);

        node->result_type = node->result_type; // Already set in create_number_literal_node
        advance(parser);
    }
    else if (parser->current_token.type == TOKEN_IDENTIFIER)
    {
        node = create_identifier_node(parser->current_token.lexeme);
        // For now, assume TYPE_INT; later, lookup from symbol table
        node->result_type = TYPE_INT;
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
