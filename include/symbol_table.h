/****************************************
* ASSEMBLER: symbol_table.h             *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#ifndef ASSEMBLER_SYMBOL_TABLE_H
#define ASSEMBLER_SYMBOL_TABLE_H

#include <stdio.h> /* FILE */

#include "assembler_utils.h" /* Utils file */

typedef enum
{
    MACRO,
    CODE,
    DATA,
    EXTERNAL,
    ENTRY
} SymbolCharacteristic;

typedef struct
{
    char name[MAX_SENTENCE_SIZE];
    SymbolCharacteristic type;
    int value;
} Symbol;

typedef struct node
{
    Symbol *symbol;
    struct node *next;
} SymbolTableNode;

void GetSymbolDetails(SymbolTableNode *symbolTableHead,
                      const char *symbolName,
                      Symbol *symbol,
                      bool *errorHasOccurred,
                      int lineNumber);
bool IsValidMacro(const SymbolTableNode *symbolTableHead,
                  const char *macroName,
                  int *value,
                  bool *errorHasOccurred,
                  int lineNumber);
void SetSymbolParams(Symbol *symbol,
                     const char *name,
                     SymbolCharacteristic type,
                     int value);
void UpdateExternValue(SymbolTableNode *symbolTableHead,
                       const char *symbolName,
                       int newValue,
                       bool *errorHasOccurred,
                       int lineNumber);
void UpdateDataSymbols(SymbolTableNode *symbolTableHead, int valueToAdd);
void UpdateSymbolTypeToEntry(SymbolTableNode *symbolTableHead, const char *symbol);
void WriteToFileByType(FILE *file,
                       SymbolTableNode *symbolTableHead,
                       SymbolCharacteristic type);

void InsertMacroToSymbolTable(const char *macroSentence,
                              SymbolTableNode **symbolTableHead,
                              bool *errorHasOccurred,
                              int lineNumber);
void InsertSymbolToSymbolTable(const char *sentenceWithSymbol,
                               SymbolTableNode **symbolTableHead,
                               SymbolCharacteristic characteristic,
                               int counter,
                               bool *errorHasOccurred,
                               int lineNumber);
void InsertExternToSymbolTable(const char *externSentence,
                               SymbolTableNode **symbolTableHead,
                               bool *errorHasOccurred,
                               int lineNumber);

void DestroySymbolTable(SymbolTableNode *head);

#endif /* ASSEMBLER_SYMBOL_TABLE_H */
