/****************************************
* ASSEMBLER: sentence_analyzer.c        *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#include <stdio.h>  /* stderr */
#include <errno.h>  /* errno */
#include <string.h> /* strncmp, strncpy, strstr */
#include <ctype.h>  /* isspace, isalpha, isdigit */
#include <stdlib.h> /* atoi */
#include <assert.h> /* assert */

#include "sentence_analyzer.h" /* API */
#include "operations.h"        /* API */

static bool IsSpecificSentenceByPrefix(const char *sentence,
                                       const char *prefix);
static bool HasSubstring(const char *sentence, const char *substring);

bool IsCommentSentence(const char *sentence)
{
    assert(NULL != sentence);

    return (IsSpecificSentenceByPrefix(sentence, COMMENT_SENTENCE_PREFIX));
}

bool IsMacroSentence(const char *sentence)
{
    assert(NULL != sentence);

    return (IsSpecificSentenceByPrefix(sentence, MACRO_SENTENCE_PREFIX));
}

bool IsEntrySentence(const char *sentence)
{
    assert(NULL != sentence);

    return (IsSpecificSentenceByPrefix(sentence, ENTRY_SENTENCE_PREFIX));
}

bool IsExternSentence(const char *sentence)
{
    assert(NULL != sentence);

    return HasSubstring(sentence, EXTERN_SENTENCE_PREFIX);
}

bool IsEmptySentence(const char *sentence)
{
    int i = 0;

    assert(NULL != sentence);

    for (i = 0; sentence[i] != NEW_LINE; ++i)
    {
        if (!isspace(sentence[i]))
        {
            return FALSE;
        }
    }

    return TRUE;
}

bool IsDataSentence(const char *sentence)
{
    assert(NULL != sentence);

    return HasSubstring(sentence, DATA_SENTENCE_PREFIX);
}

bool IsStringSentence(const char *sentence)
{
    assert(NULL != sentence);

    return HasSubstring(sentence, STRING_SENTENCE_PREFIX);
}

bool HasValidSymbol(const char *sentence)
{
    int i = 0;

    assert(NULL != sentence);

    if (!isalpha(sentence[i])) /* Label has to start with an alphabet */
    {
        return FALSE;
    }

    for (i = 1; sentence[i] != NEW_LINE && sentence[i] != COLON_SIGN; ++i)
    {
        if (i >= MAX_LABEL_SIZE ||
            (!isalpha(sentence[i]) && !isdigit(sentence[i])))
        {
            return FALSE;
        }
    }

    return TRUE;
}

bool HasValidOperationName(const char *sentence)
{
    char operationName[MAX_SENTENCE_SIZE];

    assert(NULL != sentence);

    GetOperationName(sentence, operationName);

    return IsInOperationsTable(operationName);
}

void GetString(const char *stringSentence, char *string)
{
    int stringStartIndex = 0, stringEndIndex = 0, stringLen = 0;

    assert(NULL != stringSentence);
    assert(NULL != string);
    assert(IsStringSentence(stringSentence));

    stringStartIndex = FindChar(stringSentence, QUOTATION_MARK_SIGN) + 1;
    stringEndIndex = stringStartIndex +
                     FindChar(stringSentence + stringStartIndex,
                              QUOTATION_MARK_SIGN);
    stringLen = stringEndIndex - stringStartIndex;

    strncpy(string, stringSentence + stringStartIndex, stringLen);
    string[stringLen] = END_LINE;
}

void GetData(const char *dataSentence, char *data)
{
    char *tempData = NULL;

    assert(NULL != dataSentence);
    assert(NULL != data);
    assert(IsDataSentence(dataSentence));

    tempData = strstr(dataSentence, DATA_SENTENCE_PREFIX) +
               strlen(DATA_SENTENCE_PREFIX);

    strcpy(data, tempData);
    RemoveWhiteSpaces(data);
}

void GetSymbol(const char *sentence, char *symbol)
{
    int symbolEndIndex = FindChar(sentence, COLON_SIGN);

    assert(NULL != sentence);
    assert(NULL != symbol);
    assert(HasValidSymbol(sentence));

    strncpy(symbol, sentence, symbolEndIndex);
    symbol[symbolEndIndex] = END_LINE;
}

void GetInstructionParams(const char *instructionSentence, char *params)
{
    int runner = 0, colonIndex = 0;

    assert(NULL != instructionSentence);
    assert(NULL != params);

    colonIndex = FindChar(instructionSentence, COLON_SIGN);

    if (NOT_FOUND != colonIndex) /* Sentence has a symbol */
    {
        runner = colonIndex + 1;
    }

    while (isspace(instructionSentence[runner]))
    {
        ++runner;
    }

    while (!isspace(instructionSentence[runner]))
    {
        ++runner;
    }

    while (isspace(instructionSentence[runner]))
    {
        ++runner;
    }

    strcpy(params, instructionSentence + runner);
    RemoveWhiteSpaces(params);
}

void GetOperationName(const char *sentence, char *operationName)
{
    int operationStartIndex = 0, operationEndIndex = 0, runner = 0;
    int operationNameSize = 0, colonIndex = 0;

    assert(NULL != sentence);
    assert(NULL != operationName);

    colonIndex = FindChar(sentence, COLON_SIGN);

    if (NOT_FOUND != colonIndex) /* Sentence has a symbol */
    {
        runner = colonIndex + 1;
    }

    while (isspace(sentence[runner]))
    {
        ++runner;
    }
    operationStartIndex = runner;

    while (isalpha(sentence[runner]))
    {
        ++runner;
    }
    operationEndIndex = runner;

    operationNameSize = operationEndIndex - operationStartIndex;
    strncpy(operationName, sentence + operationStartIndex, operationNameSize);
    operationName[operationNameSize] = END_LINE;
}

/* Remove white spaces [according to isspace()] from a given string */
void RemoveWhiteSpaces(char *str)
{
    int count = 0, i = 0;

    assert(NULL != str);

    for (i = 0; str[i]; ++i)
    {
        if (!isspace(str[i]))
        {
            str[count++] = str[i];
        }
    }

    str[count] = END_LINE;
}

int FindChar(const char *str, char c)
{
    int count = 0, i = 0;

    assert(NULL != str);

    for (i = 0; str[i]; ++i, ++count)
    {
        if (str[i] == c)
        {
            return count;
        }
    }

    return NOT_FOUND;
}

/* Static functions */
static bool IsSpecificSentenceByPrefix(const char *sentence,
                                       const char *prefix)
{
    assert(NULL != sentence);
    assert(NULL != prefix);

    return (0 == strncmp(sentence, prefix, strlen(prefix)));
}

static bool HasSubstring(const char *sentence, const char *substring)
{
    assert(NULL != sentence);
    assert(NULL != substring);

    return (NULL != strstr(sentence, substring));
}
