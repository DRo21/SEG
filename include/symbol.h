/**
 * @file symbol.h
 * @brief Symbol table interface for the SEG language compiler.
 *        Manages variable names and types for semantic analysis and code generation.
 *        Supports variable lookups and insertion during parsing and code generation phases.
 * @author Dario Romandini
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"
#include "ast.h"

/**
 * @brief Structure representing a symbol table entry.
 */
typedef struct Symbol
{
    char *name;          /**< Variable name */
    VarType type;        /**< Variable type */
    struct Symbol *next; /**< Pointer to the next symbol in the table (linked list) */
} Symbol;

/**
 * @brief Adds a new symbol to the symbol table.
 * @param table Pointer to the head of the symbol table.
 * @param name The variable name.
 * @param type The variable type.
 * @return Updated head of the symbol table.
 */
Symbol *add_symbol(Symbol *table, const char *name, VarType type);

/**
 * @brief Looks up a symbol by name in the symbol table.
 * @param table Pointer to the head of the symbol table.
 * @param name The variable name to search for.
 * @return Pointer to the Symbol if found, NULL otherwise.
 */
Symbol *lookup_symbol(Symbol *table, const char *name);

/**
 * @brief Frees the entire symbol table, including all entries.
 * @param table Pointer to the head of the symbol table.
 */
void free_symbol_table(Symbol *table);

#endif // SYMBOL_H
