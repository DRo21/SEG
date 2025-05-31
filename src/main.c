/**
 * @file main.c
 * @brief Entry point for the SEG compiler.
 *        Initializes the lexer, parser, and processes SEG source code.
 *        Generates x86-64 assembly output and displays parsed AST for debugging.
 * @author Dario Romandini
 */

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "ast.h"
#include "token.h"

static void print_expression(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_LITERAL:
        printf("%s", node->literal.value);
        break;
    case AST_IDENTIFIER:
        printf("%s", node->identifier.name);
        break;
    case AST_BINARY_EXPR:
        printf("(");
        print_expression(node->binary_expr.left);
        printf(" %s ", token_type_to_string(node->binary_expr.op));
        print_expression(node->binary_expr.right);
        printf(")");
        break;
    case AST_UNARY_EXPR:
        printf("(%s ", token_type_to_string(node->unary_expr.op));
        print_expression(node->unary_expr.operand);
        printf(")");
        break;
    default:
        printf("[Unknown Expression]");
    }
}

static void print_ast(ASTNode *node)
{
    while (node)
    {
        switch (node->type)
        {
        case AST_VAR_DECL:
            printf("VarDecl: type=%d name=%s value=", node->var_decl.var_type, node->var_decl.name);
            print_expression(node->var_decl.value);
            printf("\n");
            break;
        case AST_IF_STATEMENT:
            printf("IfStatement: condition=");
            print_expression(node->if_statement.condition);
            printf("\nThen:\n");
            print_ast(node->if_statement.then_branch);
            if (node->if_statement.else_branch)
            {
                printf("Else:\n");
                print_ast(node->if_statement.else_branch);
            }
            break;
        default:
            printf("[Unknown Node]\n");
        }
        node = node->next;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <file.seg>\n", argv[0]);
        return 1;
    }

    FILE *source = fopen(argv[1], "r");
    if (!source)
    {
        perror("Failed to open source file");
        return 1;
    }

    Lexer lexer;
    lexer_init(&lexer, source);

    Parser parser;
    parser_init(&parser, &lexer);

    ASTNode *program = parse_program(&parser);

    printf("=== Parsed AST ===\n");
    print_ast(program);

    FILE *asm_file = fopen("output.s", "w");
    if (!asm_file)
    {
        perror("Failed to open output file");
        free_ast(program);
        fclose(source);
        return 1;
    }

    generate_program(program, asm_file);
    fclose(asm_file);
    free_ast(program);
    fclose(source);

    printf("Compilation successful. Assembly code generated in output.s\n");
    return 0;
}
