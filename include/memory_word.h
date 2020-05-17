/****************************************
* ASSEMBLER: memoryWord.h              *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#ifndef ASSEMBLER_MEMORY_WORD_H
#define ASSEMBLER_MEMORY_WORD_H

#include "symbol_table.h"    /* API */
#include "assembler_utils.h" /* Utils file */

#define MEMORY_WORD_SIZE_IN_BITS (14)

typedef enum
{
    IMMEDIATE_ADDRESSING = 0,
    DIRECT_ADDRESSING = 1,
    FIXED_INDEX_ADDRESSING = 2,
    DIRECT_REGISTER_ADDRESSING = 3
} AddressingMethods;

typedef enum
{
    ABSOLUTE_ENCODING = 0,
    EXTERNAL_ENCODING = 1,
    RELOCATABLE_ENCODING = 2
} Encoding;

typedef enum
{
    SRC_OPERAND,
    DEST_OPERAND
} OperandType;

typedef struct
{
    unsigned int data : MEMORY_WORD_SIZE_IN_BITS;
} MemoryWord;

typedef struct
{
    char operandStr[MAX_SENTENCE_SIZE];
    AddressingMethods addressingMethod;
    Encoding encodingType;
    unsigned int numOfMemoryWords;
    int value;
} Operand;

typedef struct
{
    char operationName[MAX_SENTENCE_SIZE];
    unsigned int operationCode;
    unsigned int numOfOperands;
    unsigned int numOfMemoryWords;
    Operand srcOperand;
    Operand destOperand;
} InstructionDetails;

void InsertToDataArray(MemoryWord *dataArray,
                       const char *sentence,
                       int *dataCounter,
                       SymbolTableNode *symbolTableHead,
                       bool *errorHasOccurred,
                       int lineNumber);
void BuildFirstMemoryWord(MemoryWord *instructionsArray,
                          const char *instructionSentence,
                          int *instructionCounter,
                          SymbolTableNode *symbolTableHead,
                          bool *errorHasOccurred,
                          int lineNumber);
void BuildOtherMemoryWords(MemoryWord *instructionsArray,
                           const char *instructionSentence,
                           int *instructionCounter,
                           SymbolTableNode *symbolTableHead,
                           bool *errorHasOccurred,
                           int lineNumber);

#endif /* ASSEMBLER_MEMORY_WORD_H */
