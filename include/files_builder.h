/****************************************
* ASSEMBLER: files_builder.h            *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#ifndef ASSEMBLER_FILES_BUILDER_H
#define ASSEMBLER_FILES_BUILDER_H

#include "symbol_table.h" /* API */
#include "memory_word.h"  /* API */

void BuildFiles(MemoryWord *instructionsArray,
                MemoryWord *dataArray,
                SymbolTableNode *symbolTableHead,
                const char *filename,
                bool hasEntries,
                bool hasExternals,
                int dataCounter,
                int instructionCounter);

#endif /* ASSEMBLER_FILES_BUILDER_H */
