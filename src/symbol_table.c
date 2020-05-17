/****************************************
* ASSEMBLER: symbol_table.c             *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#include <stdlib.h> /* malloc, free */
#include <string.h> /* strcmp, symbol */
#include <assert.h> /* assert */
#include <stdio.h>  /* fprintf */

#include "symbol_table.h"      /* API */
#include "sentence_analyzer.h" /* API */

typedef struct macroDetails
{
    char name[MAX_SENTENCE_SIZE];
    int value;
} MacroDetails;

static SymbolTableNode *CreateSymbolTableNode(const Symbol *symbol);
static void DestroySymbolTableNode(SymbolTableNode *nodeToDestroy);
static void GetMacroDetails(const char *macroSentence, MacroDetails *macroDetails);
static void InsertToSymbolTable(SymbolTableNode **symbolTableHead,
                                Symbol *newSymbol,
                                bool *errorHasOccurred,
                                int lineNumber);
static void InsertNodeToTable(SymbolTableNode **symbolTableHead,
                              SymbolTableNode *nodeToInsert,
                              bool *errorHasOccurred,
                              int lineNumber);

void DestroySymbolTable(SymbolTableNode *head)
{
    SymbolTableNode *currentNode = head, *nextNode = NULL;

    while (NULL != currentNode)
    {
        nextNode = currentNode->next;
        DestroySymbolTableNode(currentNode);
        currentNode = nextNode;
    }
}

void GetSymbolDetails(SymbolTableNode *symbolTableHead,
                      const char *symbolName,
                      Symbol *symbol,
                      bool *errorHasOccurred,
                      int lineNumber)
{
    const SymbolTableNode *currentNode = symbolTableHead;

    while (NULL != currentNode)
    {
        if (0 == strcmp(currentNode->symbol->name, symbolName))
        {
            if (EXTERNAL == currentNode->symbol->type &&
                currentNode->symbol->value != 0)
            {
                Symbol newSymbol = {0};

                SetSymbolParams(&newSymbol, symbolName, EXTERNAL, 0);
                InsertToSymbolTable(&symbolTableHead,
                                    &newSymbol,
                                    errorHasOccurred,
                                    lineNumber);
            }

            SetSymbolParams(symbol,
                            currentNode->symbol->name,
                            currentNode->symbol->type,
                            currentNode->symbol->value);

            return;
        }

        currentNode = currentNode->next;
    }

    fprintf(stderr,
            "Line %d:\tError: \"%s\" is undefined (not in symbol table)\n",
            lineNumber,
            symbolName);

    *errorHasOccurred = TRUE;
}

bool IsValidMacro(const SymbolTableNode *symbolTableHead,
                  const char *macroName,
                  int *value,
                  bool *errorHasOccurred,
                  int lineNumber)
{
    const SymbolTableNode *currentNode = symbolTableHead;

    while (NULL != currentNode)
    {
        if (0 == strcmp(currentNode->symbol->name, macroName))
        {
            if (MACRO == currentNode->symbol->type)
            {
                *value = currentNode->symbol->value;

                return TRUE;
            }
            else
            {
                fprintf(stderr,
                        "Line %d:\tError: \"%s\" is not characterized as macro\n",
                        lineNumber,
                        macroName);

                *errorHasOccurred = TRUE;

                return FALSE;
            }
        }

        currentNode = currentNode->next;
    }

    fprintf(stderr,
            "Line %d:\tError: \"%s\" is undefined (not in symbol table)\n",
            lineNumber,
            macroName);

    *errorHasOccurred = TRUE;

    return FALSE;
}

void SetSymbolParams(Symbol *symbol,
                     const char *name,
                     SymbolCharacteristic type,
                     int value)
{
    assert(NULL != symbol);
    assert(NULL != name);

    strcpy(symbol->name, name);
    symbol->type = type;
    symbol->value = value;
}

void UpdateExternValue(SymbolTableNode *symbolTableHead,
                       const char *symbolName,
                       int newValue,
                       bool *errorHasOccurred,
                       int lineNumber)
{
    SymbolTableNode *currentNode = symbolTableHead;

    assert(NULL != symbolTableHead);
    assert(NULL != symbolName);

    while (NULL != currentNode)
    {
        if (0 == strcmp(currentNode->symbol->name, symbolName) &&
            0 == currentNode->symbol->value) /* Value not initialized yet */
        {
            currentNode->symbol->value = newValue;
            return;
        }

        currentNode = currentNode->next;
    }
}

void UpdateDataSymbols(SymbolTableNode *symbolTableHead, int valueToAdd)
{
    SymbolTableNode *currentNode = symbolTableHead;

    assert(NULL != symbolTableHead);

    while (NULL != currentNode)
    {
        if (DATA == currentNode->symbol->type)
        {
            currentNode->symbol->value += valueToAdd;
        }

        currentNode = currentNode->next;
    }
}

void UpdateSymbolTypeToEntry(SymbolTableNode *symbolTableHead, const char *symbol)
{
    SymbolTableNode *currentNode = symbolTableHead;

    assert(NULL != symbolTableHead);
    assert(NULL != symbol);

    while (NULL != currentNode)
    {
        if (0 == strcmp(currentNode->symbol->name, symbol))
        {
            currentNode->symbol->type = ENTRY;
            return;
        }

        currentNode = currentNode->next;
    }
}

void WriteToFileByType(FILE *file,
                       SymbolTableNode *symbolTableHead,
                       SymbolCharacteristic type)
{
    SymbolTableNode *currentNode = symbolTableHead;

    assert(NULL != file);
    assert(NULL != symbolTableHead);

    while (NULL != currentNode)
    {
        if (type == currentNode->symbol->type)
        {
            fprintf(file, "%s\t%04d\n",
                    currentNode->symbol->name,
                    currentNode->symbol->value);
        }

        currentNode = currentNode->next;
    }
}

void InsertMacroToSymbolTable(const char *macroSentence,
                              SymbolTableNode **symbolTableHead,
                              bool *errorHasOccurred,
                              int lineNumber)
{
    MacroDetails macroDetails = {0};
    Symbol newSymbol = {0};

    assert(NULL != macroSentence);
    assert(IsMacroSentence(macroSentence));
    assert(NULL != symbolTableHead);
    assert(NULL != errorHasOccurred);
    assert(lineNumber >= 0);

    GetMacroDetails(macroSentence, &macroDetails);
    SetSymbolParams(&newSymbol,
                    macroDetails.name,
                    MACRO,
                    macroDetails.value);

    InsertToSymbolTable(symbolTableHead,
                        &newSymbol,
                        errorHasOccurred,
                        lineNumber);
}

void InsertSymbolToSymbolTable(const char *sentenceWithSymbol,
                               SymbolTableNode **symbolTableHead,
                               SymbolCharacteristic characteristic,
                               int counter,
                               bool *errorHasOccurred,
                               int lineNumber)
{
    Symbol newSymbol = {0};
    char symbolName[MAX_SENTENCE_SIZE];

    assert(NULL != sentenceWithSymbol);
    assert(HasValidSymbol(sentenceWithSymbol));
    assert(NULL != symbolTableHead);
    assert(counter >= 0);
    assert(NULL != errorHasOccurred);
    assert(lineNumber >= 0);

    GetSymbol(sentenceWithSymbol, symbolName);
    SetSymbolParams(&newSymbol, symbolName, characteristic, counter);

    InsertToSymbolTable(symbolTableHead,
                        &newSymbol,
                        errorHasOccurred,
                        lineNumber);
}

void InsertExternToSymbolTable(const char *externSentence,
                               SymbolTableNode **symbolTableHead,
                               bool *errorHasOccurred,
                               int lineNumber)
{
    Symbol newSymbol = {0};
    char extern_symbol[MAX_SENTENCE_SIZE];
    char *externPrefixEnd = NULL;

    assert(NULL != externSentence);
    assert(IsExternSentence(externSentence));
    assert(NULL != symbolTableHead);

    externPrefixEnd = strstr(externSentence, EXTERN_SENTENCE_PREFIX) +
                      strlen(EXTERN_SENTENCE_PREFIX);

    strcpy(extern_symbol, externPrefixEnd);
    RemoveWhiteSpaces(extern_symbol);
    SetSymbolParams(&newSymbol, extern_symbol, EXTERNAL, 0);

    InsertToSymbolTable(symbolTableHead,
                        &newSymbol,
                        errorHasOccurred,
                        lineNumber);
}

/* Static functions */
static void InsertNodeToTable(SymbolTableNode **symbolTableHead,
                              SymbolTableNode *nodeToInsert,
                              bool *errorHasOccurred,
                              int lineNumber)
{
    SymbolTableNode *currentNode = *symbolTableHead, *lastNode = NULL;

    assert(NULL != symbolTableHead);
    assert(NULL != nodeToInsert);

    if (NULL == *symbolTableHead) /* Empty table */
    {
        *symbolTableHead = nodeToInsert;
        return;
    }

    assert(NULL != *symbolTableHead);

    while (NULL != currentNode)
    {
        lastNode = currentNode;

        if (0 == strcmp(currentNode->symbol->name, nodeToInsert->symbol->name) &&
            currentNode->symbol->type != EXTERNAL)
        {
            fprintf(stderr, "Line %d:\tError: redefinition of \"%s\"\n",
                    lineNumber,
                    currentNode->symbol->name);
            DestroySymbolTableNode(nodeToInsert);
            *errorHasOccurred = TRUE;

            return;
        }

        currentNode = currentNode->next;
    }

    lastNode->next = nodeToInsert;
}

static void DestroySymbolTableNode(SymbolTableNode *nodeToDestroy)
{
    free(nodeToDestroy->symbol);
    free(nodeToDestroy);
}

static void GetMacroDetails(const char *macroSentence,
                            MacroDetails *macroDetails)
{
    int equalSignIndex = 0;
    char tempMacroSentence[MAX_SENTENCE_SIZE];

    strcpy(tempMacroSentence, macroSentence + strlen(MACRO_SENTENCE_PREFIX));
    RemoveWhiteSpaces(tempMacroSentence);

    equalSignIndex = FindChar(tempMacroSentence, EQUAL_SIGN);

    if (NOT_FOUND != equalSignIndex)
    {
        strncpy(macroDetails->name, tempMacroSentence, equalSignIndex);
        macroDetails->value = atoi(tempMacroSentence + equalSignIndex + 1);
    }
}

static void InsertToSymbolTable(SymbolTableNode **symbolTableHead,
                                Symbol *newSymbol,
                                bool *errorHasOccurred,
                                int lineNumber)
{
    SymbolTableNode *newNode = CreateSymbolTableNode(newSymbol);

    if (NULL != newNode)
    {
        InsertNodeToTable(symbolTableHead,
                          newNode,
                          errorHasOccurred,
                          lineNumber);
    }
    else
    {
        fprintf(stderr, "Line %d:\tMemory allocation error\n", lineNumber);
        *errorHasOccurred = TRUE;
    }
}

static SymbolTableNode *CreateSymbolTableNode(const Symbol *symbol)
{
    SymbolTableNode *newNode = NULL;

    newNode = (SymbolTableNode *)malloc(sizeof(SymbolTableNode));
    if (NULL == newNode)
    {
        return NULL;
    }

    newNode->symbol = (Symbol *)malloc(sizeof(Symbol));
    if (NULL == newNode->symbol)
    {
        free(newNode);
        return NULL;
    }

    memcpy(newNode->symbol, symbol, sizeof(Symbol));
    newNode->next = NULL;

    return newNode;
}
