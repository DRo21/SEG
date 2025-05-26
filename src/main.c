/**
 * @file main.c
 * @brief Entry point for the SEG language compiler.
 *        Initializes the lexer and parser, processes SEG source code,
 *        and prints the parsed Abstract Syntax Tree (AST).
 * @author Dario Romandini
 */

#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "codegen.h"

// Recursively print an expression tree
void print_expression(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case AST_NUMBER_LITERAL:
        printf("%s", node->literal);
        break;
    case AST_IDENTIFIER:
        printf("%s", node->name);
        break;
    case AST_BINARY_EXPR:
        printf("(");
        print_expression(node->left);
        printf(" %s ", token_type_to_string(node->op));
        print_expression(node->right);
        printf(")");
        break;
    default:
        printf("[Unknown Expression]");
    }
}

/**
 * @brief Main entry point for the SEG compiler.
 *
 * Usage: ./seg <source.seg>
 *
 * @param argc Number of command-line arguments
 * @param argv Command-line argument values
 * @return Exit code (0 for success, 1 for failure)
 */
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
        perror("Failed to open file");
        return 1;
    }

    // Initialize lexer and parser
    Lexer lexer;
    lexer_init(&lexer, source);

    Parser parser;
    parser_init(&parser, &lexer);

    // Parse the SEG program into an AST
    ASTNode *program = parse_program(&parser);

    // Print the AST
    printf("=== Parsed AST ===\n");
    ASTNode *node = program;
    while (node)
    {
        if (node->type == AST_VAR_DECL)
        {
            printf("VarDecl: type=%s name=%s value=",
                   node->var_type == TYPE_INT ? "int" : "float",
                   node->name);
            print_expression(node->value);
            printf("\n");
        }
        node = node->next;
    }

    FILE *asm_file = fopen("output.s", "w");
    if (!asm_file)
    {
        perror("Failed to open output file");
        return 1;
    }
    generate_program(program, asm_file);
    fclose(asm_file);

    // Clean up
    free_ast(program);
    fclose(source);
    return 0;
}
