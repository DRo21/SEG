/**
 * @file symbol.c
 * @brief Symbol table implementation for SEG compiler.
 * @author Dario Romandini
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "symbol.h"

Symbol *add_symbol(Symbol *head, const char *name, VarType type)
{
    Symbol *sym = (Symbol *)malloc(sizeof(Symbol));
    if (!sym)
    {
        fprintf(stderr, "Error: Failed to allocate memory for symbol.\n");
        exit(1);
    }
    sym->name = strdup(name);
    sym->type = type;
    sym->next = head;
    return sym;
}

Symbol *lookup_symbol(Symbol *head, const char *name)
{
    while (head)
    {
        if (strcmp(head->name, name) == 0)
        {
            return head;
        }
        head = head->next;
    }
    return NULL;
}

void free_symbol_table(Symbol *head)
{
    while (head)
    {
        Symbol *next = head->next;
        free(head->name);
        free(head);
        head = next;
    }
}
