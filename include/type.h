/**
 * @file type.h
 * @brief Data type definitions for the SEG language compiler.
 *        Defines the VarType enum used across AST and Symbol Table.
 * @author Dario Romandini
 */

#ifndef TYPE_H
#define TYPE_H

/**
 * @brief Supported data types in SEG.
 */
typedef enum
{
    TYPE_UNKNOWN,
    TYPE_INT,   /**< Integer */
    TYPE_FLOAT, /**< Floating-point */
    TYPE_BOOL,  /**< Boolean */
    TYPE_CHAR,  /**< Character */
    TYPE_STRING /**< String */
} VarType;

#endif // TYPE_H
