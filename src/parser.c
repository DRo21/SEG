/**
 * @file parser.c
 * @brief Parser implementation for the SEG language compiler.
 *        Handles variable declarations, expressions, literals, type checking, logical, equality, and comparison operators.
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "symbol.h"

static void advance(Parser *parser) {
    token_free(&parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
}

static void expect(Parser *parser, TokenType type) {
    if (parser->current_token.type != type) {
        printf("[Parser Error] Expected %s, got %s (line %d)\n",
               token_type_to_string(type),
               token_type_to_string(parser->current_token.type),
               parser->current_token.line);
        exit(1);
    }
}

ASTNode *parse_expression(Parser *parser);

ASTNode *parse_logical_or(Parser *parser);

ASTNode *parse_logical_xor(Parser *parser);

ASTNode *parse_logical_and(Parser *parser);

ASTNode *parse_equality(Parser *parser);

ASTNode *parse_comparison(Parser *parser);

ASTNode *parse_term(Parser *parser);

ASTNode *parse_unary(Parser *parser);

ASTNode *parse_factor(Parser *parser);

void parser_init(Parser *parser, Lexer *lexer) {
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
}

ASTNode *parse_var_decl(Parser *parser) {
    VarType var_type;

    switch (parser->current_token.type) {
        case TOKEN_INT: var_type = TYPE_INT;
            break;
        case TOKEN_FLOAT: var_type = TYPE_FLOAT;
            break;
        case TOKEN_BOOL: var_type = TYPE_BOOL;
            break;
        case TOKEN_CHAR: var_type = TYPE_CHAR;
            break;
        case TOKEN_STRING: var_type = TYPE_STRING;
            break;
        default:
            printf("[Parser Error] Expected type keyword, got %s\n",
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

    if (var_type == TYPE_BOOL) {
        value->result_type = TYPE_BOOL;
    } else if (var_type == TYPE_INT || var_type == TYPE_FLOAT) {
        if (value->result_type == TYPE_BOOL) {
            value->result_type = TYPE_INT;
        }
    }

    if (value->result_type != var_type) {
        printf("[Parser Warning] Type mismatch in assignment to '%s': declared %s, assigned %s (line %d).\n",
               name, token_type_to_string(var_type), token_type_to_string(value->result_type),
               parser->current_token.line);
    }

    expect(parser, TOKEN_SEMICOLON);
    advance(parser);

    return create_var_decl_node(var_type, name, value);
}

ASTNode *parse_expression(Parser *parser) {
    return parse_logical_or(parser);
}

ASTNode *parse_logical_or(Parser *parser) {
    ASTNode *node = parse_logical_xor(parser);
    while (parser->current_token.type == TOKEN_OR) {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *right = parse_logical_xor(parser);
        node = create_binary_expr_node(op, node, right);
        node->result_type = TYPE_BOOL;
    }
    return node;
}

ASTNode *parse_logical_xor(Parser *parser) {
    ASTNode *node = parse_logical_and(parser);
    while (parser->current_token.type == TOKEN_XOR) {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *right = parse_logical_and(parser);
        node = create_binary_expr_node(op, node, right);
        node->result_type = TYPE_BOOL;
    }
    return node;
}

ASTNode *parse_logical_and(Parser *parser) {
    ASTNode *node = parse_equality(parser);
    while (parser->current_token.type == TOKEN_AND) {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *right = parse_equality(parser);
        node = create_binary_expr_node(op, node, right);
        node->result_type = TYPE_BOOL;
    }
    return node;
}

ASTNode *parse_equality(Parser *parser) {
    ASTNode *node = parse_comparison(parser);
    while (parser->current_token.type == TOKEN_EQ || parser->current_token.type == TOKEN_NEQ) {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *right = parse_comparison(parser);
        node = create_binary_expr_node(op, node, right);
        node->result_type = TYPE_BOOL;
    }
    return node;
}

ASTNode *parse_comparison(Parser *parser) {
    ASTNode *node = parse_term(parser);
    while (parser->current_token.type == TOKEN_LT || parser->current_token.type == TOKEN_GT ||
           parser->current_token.type == TOKEN_LEQ || parser->current_token.type == TOKEN_GEQ) {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *right = parse_term(parser);
        node = create_binary_expr_node(op, node, right);
        node->result_type = TYPE_BOOL;
    }
    return node;
}

ASTNode *parse_term(Parser *parser) {
    ASTNode *node = parse_unary(parser);
    while (parser->current_token.type == TOKEN_PLUS || parser->current_token.type == TOKEN_MINUS ||
           parser->current_token.type == TOKEN_STAR || parser->current_token.type == TOKEN_SLASH) {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *right = parse_unary(parser);

        if (node->result_type != right->result_type) {
            node->result_type = TYPE_FLOAT;
            right->result_type = TYPE_FLOAT;
        }

        node = create_binary_expr_node(op, node, right);
        node->result_type = right->result_type;
    }
    return node;
}

ASTNode *parse_unary(Parser *parser) {
    if (parser->current_token.type == TOKEN_NOT) {
        TokenType op = parser->current_token.type;
        advance(parser);
        ASTNode *operand = parse_unary(parser);
        return create_unary_expr_node(op, operand);
    }
    return parse_factor(parser);
}

ASTNode *parse_factor(Parser *parser) {
    ASTNode *node = NULL;

    switch (parser->current_token.type) {
        case TOKEN_NUMBER:
            node = create_literal_node(parser->current_token.lexeme,
                                       strchr(parser->current_token.lexeme, '.') ? TYPE_FLOAT : TYPE_INT);
            advance(parser);
            break;
        case TOKEN_BOOL_LITERAL:
            node = create_literal_node(parser->current_token.lexeme, TYPE_BOOL);
            advance(parser);
            break;
        case TOKEN_CHAR_LITERAL:
            node = create_literal_node(parser->current_token.lexeme, TYPE_CHAR);
            advance(parser);
            break;
        case TOKEN_STRING_LITERAL:
            node = create_literal_node(parser->current_token.lexeme, TYPE_STRING);
            advance(parser);
            break;
        case TOKEN_IDENTIFIER:
            node = create_identifier_node(parser->current_token.lexeme);
            advance(parser);
            break;
        case TOKEN_LPAREN:
            advance(parser);
            node = parse_expression(parser);
            expect(parser, TOKEN_RPAREN);
            advance(parser);
            break;
        default:
            printf("[Parser Error] Unexpected token: %s\n",
                   token_type_to_string(parser->current_token.type));
            exit(1);
    }

    return node;
}

ASTNode *parse_program(Parser *parser) {
    ASTNode *head = NULL, *current = NULL;

    while (parser->current_token.type != TOKEN_EOF) {
        ASTNode *node = parse_var_decl(parser);
        if (!head) head = node;
        else current->next = node;
        current = node;
    }

    return head;
}
