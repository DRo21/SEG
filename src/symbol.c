/**
 * @file symbol.c
 * @brief Implementation of the symbol table for the SEG language compiler.
 *        Provides functions to add, lookup, and free variable symbols used in parsing and code generation.
 *        The symbol table is implemented as a linked list for simplicity.
 * @author Dario Romandini
 */

#include <stdlib.h>
#include <string.h>
#include "symbol.h"

Symbol *add_symbol(Symbol *table, const char *name, VarType type)
{
    Symbol *new_symbol = malloc(sizeof(Symbol));
    new_symbol->name = strdup(name);
    new_symbol->type = type;
    new_symbol->next = table;
    return new_symbol;
}

Symbol *lookup_symbol(Symbol *table, const char *name)
{
    for (Symbol *s = table; s; s = s->next)
    {
        if (strcmp(s->name, name) == 0)
        {
            return s;
        }
    }
    return NULL;
}

void free_symbol_table(Symbol *table)
{
    while (table)
    {
        Symbol *next = table->next;
        free(table->name);
        free(table);
        table = next;
    }
}
