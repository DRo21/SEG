/**
 * @file symbol.h
 * @brief Symbol table for SEG compiler. Maps variable names to assembly labels and types.
 * @author Dario Romandini
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include "ast.h"

typedef struct Symbol {
    char *name; ///< Variable name (e.g., "x")
    VarType type; ///< Variable type (int/float)
    struct Symbol *next; ///< Linked list pointer
} Symbol;

/**
 * @brief Add a variable to the symbol table.
 *
 * @param head Pointer to the symbol table (linked list head)
 * @param name Variable name
 * @param type Variable type (int/float)
 * @return Updated head of the symbol table
 */
Symbol *add_symbol(Symbol *head, const char *name, VarType type);

/**
 * @brief Lookup a variable in the symbol table.
 *
 * @param head Pointer to the symbol table (linked list head)
 * @param name Variable name
 * @return Pointer to the symbol if found, or NULL
 */
Symbol *lookup_symbol(Symbol *head, const char *name);

/**
 * @brief Free the symbol table.
 *
 * @param head Pointer to the symbol table
 */
void free_symbol_table(Symbol *head);

#endif // SYMBOL_H
