/****************************************
* ASSEMBLER: operations.c               *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#include <string.h> /* strcmp */
#include <assert.h> /* assert */

#include "operations.h" /* API */

typedef struct operation
{
    char name[MAX_SENTENCE_SIZE];
    unsigned int code;
} Operation;

const Operation OperationsTable[NUM_OF_OPERATIONS] = {
    {"mov", 0},
    {"cmp", 1},
    {"add", 2},
    {"sub", 3},
    {"not", 4},
    {"clr", 5},
    {"lea", 6},
    {"inc", 7},
    {"dec", 8},
    {"jmp", 9},
    {"bne", 10},
    {"red", 11},
    {"prn", 12},
    {"jsr", 13},
    {"rts", 14},
    {"stop", 15}};

bool IsInOperationsTable(const char *operationName)
{
    int i = 0;

    assert(NULL != operationName);

    for (i = 0; i < NUM_OF_OPERATIONS; ++i)
    {
        if (0 == strcmp(OperationsTable[i].name, operationName))
        {
            return TRUE;
        }
    }

    return FALSE;
}

int GetOperationCode(const char *operationName)
{
    int i = 0, returnCode = 0;

    assert(NULL != operationName);
    assert(IsInOperationsTable(operationName));

    for (i = 0; i < NUM_OF_OPERATIONS; ++i)
    {
        if (0 == strcmp(OperationsTable[i].name, operationName))
        {
            returnCode = OperationsTable[i].code;
            break;
        }
    }

    return returnCode;
}

int GetNumOfOperands(const char *operationName)
{
    int operationCode = 0, result = 0;

    assert(NULL != operationName);
    assert(IsInOperationsTable(operationName));

    operationCode = GetOperationCode(operationName);

    switch (operationCode)
    {
    /* No break's on purpose */
    case (0):
    case (1):
    case (2):
    case (3):
    case (6):
        result = 2;
        break;

    /* No break's on purpose */
    case (4):
    case (5):
    case (7):
    case (8):
    case (9):
    case (10):
    case (11):
    case (12):
    case (13):
        result = 1;
        break;

    /* No break's on purpose */
    case (14):
    case (15):
        result = 0;
        break;

    default:
        break;
    }

    return result;
}