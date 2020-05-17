/****************************************
* ASSEMBLER: memoryWord.c              *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#include <string.h> /* strlen, strtok */
#include <assert.h> /* assert */
#include <ctype.h>  /* isdigit, isalpha */
#include <stdlib.h> /* atoi */

#include "memory_word.h"        /* API */
#include "sentence_analyzer.h" /* API */
#include "operations.h"        /* API */

static void InsertStringToDataArray(MemoryWord *dataArray,
                                    const char *sentence,
                                    int *dataCounter);
static void InsertDataToDataArray(MemoryWord *dataArray,
                                  const char *sentence,
                                  int *dataCounter,
                                  SymbolTableNode *symbolTableHead,
                                  bool *errorHasOccurred,
                                  int lineNumber);
static void SetMemoryWord(MemoryWord *memoryWord, unsigned int data);
static bool IsNumber(const char *str);
static bool IsImmediateNumber(const char *operand);
static bool IsRegister(const char *operand);
static int GetRegisterNum(const char *registerOperand);
static bool IsFixedIndex(const char *operand);
static void GetInstructionDetails(const char *instructionSentence,
                                  InstructionDetails *instructionDetails,
                                  SymbolTableNode *symbolTableHead,
                                  bool *errorHasOccurred,
                                  int lineNumber);
static void BuildMemoryWordsForOperand(Operand *operand,
                                       MemoryWord *instructionsArray,
                                       int *instructionCounter,
                                       SymbolTableNode *symbolTableHead,
                                       bool *errorHasOccurred,
                                       int lineNumber,
                                       OperandType operandType);
static void FillOperandDetails(Operand *operand,
                               SymbolTableNode *symbolTableHead,
                               bool *errorHasOccurred,
                               int lineNumber);
static int GetNumberOrMacroValue(const char *operand,
                                 SymbolTableNode *symbolTableHead,
                                 bool *errorHasOccurred,
                                 int lineNumber);
static void GetValueBetweenBrackets(const char *operand,
                                    char *valueBetweenSquareBrackets);
static void SetMemoryWordWithSymbol(const char *symbolName,
                                    MemoryWord *instructionsArray,
                                    int *instructionCounter,
                                    SymbolTableNode *symbolTableHead,
                                    bool *errorHasOccurred,
                                    int lineNumber);
static void SetMemoryWordWithValueAndEncoding(MemoryWord *instructionsArray,
                                              int *instructionCounter,
                                              int value,
                                              Encoding encodingType);

void InsertToDataArray(MemoryWord *dataArray,
                       const char *sentence,
                       int *dataCounter,
                       SymbolTableNode *symbolTableHead,
                       bool *errorHasOccurred,
                       int lineNumber)
{
    assert(NULL != dataArray);
    assert(NULL != sentence);
    assert(IsDataSentence(sentence) || IsStringSentence(sentence));
    assert(NULL != dataCounter);
    assert(NULL != symbolTableHead);
    assert(NULL != errorHasOccurred);
    assert(lineNumber >= 0);

    IsStringSentence(sentence)
        ? InsertStringToDataArray(dataArray, sentence, dataCounter)
        : InsertDataToDataArray(dataArray,
                                sentence,
                                dataCounter,
                                symbolTableHead,
                                errorHasOccurred,
                                lineNumber);
}

void BuildFirstMemoryWord(MemoryWord *instructionsArray,
                          const char *instructionSentence,
                          int *instructionCounter,
                          SymbolTableNode *symbolTableHead,
                          bool *errorHasOccurred,
                          int lineNumber)
{
    InstructionDetails instructionDetails = {0};
    MemoryWord *memoryWord = NULL;
    unsigned int data = 0;

    assert(NULL != instructionsArray);
    assert(NULL != instructionSentence);
    assert(NULL != instructionCounter);
    assert(NULL != symbolTableHead);
    assert(NULL != errorHasOccurred);
    assert(lineNumber >= 0);

    memoryWord = (MemoryWord *)(instructionsArray + *instructionCounter);

    GetInstructionDetails(instructionSentence,
                          &instructionDetails,
                          symbolTableHead,
                          errorHasOccurred,
                          lineNumber);

    /* build first memory word */
    data = (instructionDetails.operationCode << 6) |
           (instructionDetails.srcOperand.addressingMethod << 4) |
           (instructionDetails.destOperand.addressingMethod << 2);
    SetMemoryWord(memoryWord, data);

    /* Update IC */
    *instructionCounter += instructionDetails.numOfMemoryWords;
}

void BuildOtherMemoryWords(MemoryWord *instructionsArray,
                           const char *instructionSentence,
                           int *instructionCounter,
                           SymbolTableNode *symbolTableHead,
                           bool *errorHasOccurred,
                           int lineNumber)
{
    InstructionDetails instructionDetails = {0};

    assert(NULL != instructionsArray);
    assert(NULL != instructionSentence);
    assert(NULL != instructionCounter);
    assert(NULL != symbolTableHead);
    assert(NULL != errorHasOccurred);
    assert(lineNumber >= 0);

    GetInstructionDetails(instructionSentence,
                          &instructionDetails,
                          symbolTableHead,
                          errorHasOccurred,
                          lineNumber);

    ++(*instructionCounter); /* For the first word */

    if (DIRECT_REGISTER_ADDRESSING == instructionDetails.srcOperand.addressingMethod &&
        DIRECT_REGISTER_ADDRESSING == instructionDetails.destOperand.addressingMethod)
    {
        MemoryWord *memoryWord = (MemoryWord *)(instructionsArray + *instructionCounter);
        unsigned int data = (instructionDetails.srcOperand.value << 5) |
                            (instructionDetails.destOperand.value << 2);

        SetMemoryWord(memoryWord, data);
        ++(*instructionCounter);
    }
    else
    {
        BuildMemoryWordsForOperand(&instructionDetails.srcOperand,
                                   instructionsArray,
                                   instructionCounter,
                                   symbolTableHead,
                                   errorHasOccurred,
                                   lineNumber,
                                   SRC_OPERAND);

        BuildMemoryWordsForOperand(&instructionDetails.destOperand,
                                   instructionsArray,
                                   instructionCounter,
                                   symbolTableHead,
                                   errorHasOccurred,
                                   lineNumber,
                                   DEST_OPERAND);
    }
}

/* Static functions */
static void InsertStringToDataArray(MemoryWord *dataArray,
                                    const char *sentence,
                                    int *dataCounter)
{
    char string[MAX_SENTENCE_SIZE] = {0};
    int i = 0, stringLen = 0;

    GetString(sentence, string);
    stringLen = strlen(string) + 1; /* +1 for '\0' */

    for (i = 0; i < stringLen; ++i, ++(*dataCounter))
    {
        SetMemoryWord(dataArray + (*dataCounter), string[i]);
    }
}

static void InsertDataToDataArray(MemoryWord *dataArray,
                                  const char *sentence,
                                  int *dataCounter,
                                  SymbolTableNode *symbolTableHead,
                                  bool *errorHasOccurred,
                                  int lineNumber)
{
    char data[MAX_SENTENCE_SIZE] = {0};
    char *token = NULL;

    assert(IsDataSentence(sentence));

    GetData(sentence, data);
    token = strtok(data, COMMA_SIGN_ARRAY);

    while (NULL != token)
    {
        int macroValue = 0;

        if (IsNumber(token))
        {
            SetMemoryWord(dataArray + (*dataCounter)++, atoi(token));
        }
        else if (IsValidMacro(symbolTableHead,
                              token,
                              &macroValue,
                              errorHasOccurred,
                              lineNumber))
        {
            SetMemoryWord(dataArray + (*dataCounter)++, macroValue);
        }
        else
        {
            return;
        }

        token = strtok(NULL, COMMA_SIGN_ARRAY);
    }
}

static bool IsNumber(const char *str)
{
    return (str[0] == PLUS_SIGN || str[0] == MINUS_SIGN || isdigit(str[0]));
}

static bool IsImmediateNumber(const char *operand)
{
    return (operand[0] == HASH_MARK);
}

static bool IsRegister(const char *operand)
{
    return (2 == strlen(operand) &&
            operand[0] == REGISTER_PREFIX &&
            isdigit(operand[1]) &&
            operand[1] != ZERO_DIGIT &&
            operand[1] != NINE_DIGIT);
}

static int GetRegisterNum(const char *registerOperand)
{
    assert(IsRegister(registerOperand));

    return registerOperand[1] - '0';
}

static bool IsFixedIndex(const char *operand)
{
    int openingSquareBracketsIndex = FindChar(operand, OPENING_SQUARE_BRACKETS);
    int closingSquareBracketsIndex = FindChar(operand, CLOSING_SQUARE_BRACKETS);

    return (NOT_FOUND != openingSquareBracketsIndex &&
            NOT_FOUND != closingSquareBracketsIndex &&
            closingSquareBracketsIndex > (openingSquareBracketsIndex + 1));
}

static int GetNumberOrMacroValue(const char *operand,
                                 SymbolTableNode *symbolTableHead,
                                 bool *errorHasOccurred,
                                 int lineNumber)
{
    int value = 0;

    if (IsNumber(operand))
    {
        value = atoi(operand);
    }
    else if (!IsValidMacro(symbolTableHead,
                           operand,
                           &value,
                           errorHasOccurred,
                           lineNumber))
    {
        value = ERROR;
    }

    return value;
}

static void GetValueBetweenBrackets(const char *operand,
                                    char *valueBetweenSquareBrackets)
{
    int openingSquareBracketsIndex = FindChar(operand, OPENING_SQUARE_BRACKETS);
    int closingSquareBracketsIndex = FindChar(operand, CLOSING_SQUARE_BRACKETS);
    int valueLen = closingSquareBracketsIndex - openingSquareBracketsIndex - 1;

    strncpy(valueBetweenSquareBrackets,
            operand + openingSquareBracketsIndex + 1,
            valueLen);
}

static void SetMemoryWordWithSymbol(const char *symbolName,
                                    MemoryWord *instructionsArray,
                                    int *instructionCounter,
                                    SymbolTableNode *symbolTableHead,
                                    bool *errorHasOccurred,
                                    int lineNumber)
{
    Symbol symbol = {0};
    SymbolCharacteristic encodingType = 0;

    GetSymbolDetails(symbolTableHead,
                     symbolName,
                     &symbol,
                     errorHasOccurred,
                     lineNumber);

    if (EXTERNAL == symbol.type)
    {
        encodingType = EXTERNAL_ENCODING;
        UpdateExternValue(symbolTableHead,
                          symbol.name,
                          *instructionCounter + STARTING_ADDRESS,
                          errorHasOccurred,
                          lineNumber);
    }
    else
    {
        encodingType = RELOCATABLE_ENCODING;
    }

    SetMemoryWordWithValueAndEncoding(instructionsArray,
                                      instructionCounter,
                                      symbol.value,
                                      encodingType);
}

static void SetMemoryWordWithValueAndEncoding(MemoryWord *instructionsArray,
                                              int *instructionCounter,
                                              int value,
                                              Encoding encodingType)
{
    MemoryWord *memoryWord =
        (MemoryWord *)(instructionsArray + *instructionCounter);
    unsigned int data = (value << 2) | encodingType;

    SetMemoryWord(memoryWord, data);
    ++(*instructionCounter);
}

static void BuildMemoryWordsForOperand(Operand *operand,
                                       MemoryWord *instructionsArray,
                                       int *instructionCounter,
                                       SymbolTableNode *symbolTableHead,
                                       bool *errorHasOccurred,
                                       int lineNumber,
                                       OperandType operandType)
{
    if (0 == operand->numOfMemoryWords)
    {
        return;
    }

    switch (operand->addressingMethod)
    {
    case IMMEDIATE_ADDRESSING:
    {
        SetMemoryWordWithValueAndEncoding(instructionsArray,
                                          instructionCounter,
                                          operand->value,
                                          operand->encodingType);

        break;
    }

    case DIRECT_ADDRESSING:
    {
        char symbolName[MAX_SENTENCE_SIZE] = {0};

        /* Set address */
        strcpy(symbolName, operand->operandStr);
        SetMemoryWordWithSymbol(symbolName,
                                instructionsArray,
                                instructionCounter,
                                symbolTableHead,
                                errorHasOccurred,
                                lineNumber);

        break;
    }

    case FIXED_INDEX_ADDRESSING:
    {
        char symbolName[MAX_SENTENCE_SIZE] = {0};
        int openingSquareBracketsIndex = 0;

        /* Set address */
        openingSquareBracketsIndex = FindChar(operand->operandStr,
                                              OPENING_SQUARE_BRACKETS);

        strncpy(symbolName, operand->operandStr, openingSquareBracketsIndex);
        symbolName[openingSquareBracketsIndex] = END_LINE;

        SetMemoryWordWithSymbol(symbolName,
                                instructionsArray,
                                instructionCounter,
                                symbolTableHead,
                                errorHasOccurred,
                                lineNumber);

        /* Set value */
        SetMemoryWordWithValueAndEncoding(instructionsArray,
                                          instructionCounter,
                                          operand->value,
                                          ABSOLUTE_ENCODING);

        break;
    }

    case DIRECT_REGISTER_ADDRESSING:
    {
        MemoryWord *memoryWord =
            (MemoryWord *)(instructionsArray + (*instructionCounter));

        if (SRC_OPERAND == operandType)
        {
            SetMemoryWord(memoryWord, (operand->value << 5) | 0);
        }
        else
        {
            assert(DEST_OPERAND == operandType);

            SetMemoryWord(memoryWord, (operand->value << 2) | 0);
        }

        ++(*instructionCounter);

        break;
    }

    default:
    {
        fprintf(stderr, "Line %d:\tError: wrong addressing method - %d\n",
                lineNumber,
                operand->addressingMethod);
        *errorHasOccurred = TRUE;
        break;
    }
    }
}

static void FillOperandDetails(Operand *operand,
                               SymbolTableNode *symbolTableHead,
                               bool *errorHasOccurred,
                               int lineNumber)
{
    if (IsImmediateNumber(operand->operandStr))
    {
        char numberOrMacro[MAX_SENTENCE_SIZE] = {0};

        operand->addressingMethod = IMMEDIATE_ADDRESSING;
        operand->encodingType = ABSOLUTE_ENCODING;

        strcpy(numberOrMacro, operand->operandStr + 1); /* +1 because of '#' */
        operand->value = GetNumberOrMacroValue(numberOrMacro,
                                               symbolTableHead,
                                               errorHasOccurred,
                                               lineNumber);
        operand->numOfMemoryWords = 1;
    }
    else if (IsRegister(operand->operandStr))
    {
        operand->addressingMethod = DIRECT_REGISTER_ADDRESSING;
        operand->encodingType = ABSOLUTE_ENCODING;
        operand->value = GetRegisterNum(operand->operandStr);
        operand->numOfMemoryWords = 1;
    }
    else if (IsFixedIndex(operand->operandStr))
    {
        char valueBetweenSquareBrackets[MAX_SENTENCE_SIZE] = {0};

        operand->addressingMethod = FIXED_INDEX_ADDRESSING;
        GetValueBetweenBrackets(operand->operandStr, valueBetweenSquareBrackets);
        operand->value = GetNumberOrMacroValue(valueBetweenSquareBrackets,
                                               symbolTableHead,
                                               errorHasOccurred,
                                               lineNumber);
        operand->numOfMemoryWords = 2;
    }
    else
    {
        operand->addressingMethod = DIRECT_ADDRESSING;
        operand->numOfMemoryWords = 1;
    }
}

static void GetInstructionDetails(const char *instructionSentence,
                                  InstructionDetails *instructionDetails,
                                  SymbolTableNode *symbolTableHead,
                                  bool *errorHasOccurred,
                                  int lineNumber)
{
    char operands[MAX_SENTENCE_SIZE] = {0};

    GetOperationName(instructionSentence, instructionDetails->operationName);
    instructionDetails->operationCode =
        GetOperationCode(instructionDetails->operationName);

    GetInstructionParams(instructionSentence, operands);
    instructionDetails->numOfOperands =
        GetNumOfOperands(instructionDetails->operationName);

    if (0 == instructionDetails->numOfOperands)
    {
        instructionDetails->numOfMemoryWords = 1;
        return;
    }

    if (2 == instructionDetails->numOfOperands)
    {
        int commaIndex = FindChar(operands, COMMA_SIGN);

        strncpy(instructionDetails->srcOperand.operandStr, operands, commaIndex);
        strcpy(instructionDetails->destOperand.operandStr, operands + commaIndex + 1);

        FillOperandDetails(&instructionDetails->srcOperand,
                           symbolTableHead,
                           errorHasOccurred,
                           lineNumber);
        FillOperandDetails(&instructionDetails->destOperand,
                           symbolTableHead,
                           errorHasOccurred,
                           lineNumber);
    }
    else
    {
        assert(1 == instructionDetails->numOfOperands);

        strcpy(instructionDetails->destOperand.operandStr, operands);
        FillOperandDetails(&instructionDetails->destOperand,
                           symbolTableHead,
                           errorHasOccurred,
                           lineNumber);
    }

    if (DIRECT_REGISTER_ADDRESSING == instructionDetails->srcOperand.addressingMethod &&
        DIRECT_REGISTER_ADDRESSING == instructionDetails->destOperand.addressingMethod)
    {
        instructionDetails->numOfMemoryWords = 2;
    }
    else
    {
        instructionDetails->numOfMemoryWords =
            instructionDetails->srcOperand.numOfMemoryWords +
            instructionDetails->destOperand.numOfMemoryWords + 1;
    }
}

static void SetMemoryWord(MemoryWord *memoryWord, unsigned int data)
{
    memoryWord->data = data;
}
