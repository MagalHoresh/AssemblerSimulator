/****************************************
* ASSEMBLER: operations.h               *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#ifndef ASSEMBLER_OPERATIONS_H
#define ASSEMBLER_OPERATIONS_H

#include "assembler_utils.h" /* Utils file */

#define NUM_OF_OPERATIONS (16)

bool IsInOperationsTable(const char *operationName);
int GetOperationCode(const char *operationName);
int GetNumOfOperands(const char *operationName);

#endif /* ASSEMBLER_OPERATIONS_H */
