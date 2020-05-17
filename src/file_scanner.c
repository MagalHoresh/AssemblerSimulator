/****************************************
* ASSEMBLER: file_scanner.c             *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#include <assert.h> /* assert */
#include <stdio.h>  /* rewind */

#include "file_scanner.h"      /* API */
#include "symbol_table.h"      /* API */
#include "sentence_analyzer.h" /* API */
#include "memory_word.h"       /* API */
#include "files_builder.h"     /* API */
#include "assembler_utils.h"   /* Utils file */

#define MEMORY_ARRAY_MAX_SIZE (1000)

static void RunFirstScan(FILE *assemblyFile,
                         SymbolTableNode **symbolTableHead,
                         const char *filename);
static void RunSecondScan(FILE *assemblyFile,
                          MemoryWord *instructionsArray,
                          MemoryWord *dataArray,
                          SymbolTableNode **symbolTableHead,
                          const char *filename,
                          bool hasEntries,
                          bool hasExternals,
                          int dataCounter);

void RunScans(FILE *assemblyFile, const char *filename)
{
    SymbolTableNode *symbolTableHead = NULL;

    assert(NULL != assemblyFile);
    assert(NULL != filename);

    RunFirstScan(assemblyFile, &symbolTableHead, filename);

    DestroySymbolTable(symbolTableHead);
}

/* Static functions */
static void RunFirstScan(FILE *assemblyFile,
                         SymbolTableNode **symbolTableHead,
                         const char *filename)
{
    MemoryWord instructionsArray[MEMORY_ARRAY_MAX_SIZE] = {0};
    MemoryWord dataArray[MEMORY_ARRAY_MAX_SIZE] = {0};
    char sentence[MAX_SENTENCE_SIZE] = {0};
    int IC = 0, DC = 0, lineNumber = 0;
    bool hasEntries = FALSE, hasExternals = FALSE, errorHasOccurred = FALSE;

    assert(NULL != assemblyFile);
    assert(NULL != symbolTableHead);

    while (fgets(sentence, MAX_SENTENCE_SIZE, (FILE *)assemblyFile))
    {
        bool hasSymbolDefinition = FALSE;

        ++lineNumber;

        if (IsEmptySentence(sentence) || IsCommentSentence(sentence))
        {
            continue;
        }

        if (IsMacroSentence(sentence))
        {
            InsertMacroToSymbolTable(sentence,
                                     symbolTableHead,
                                     &errorHasOccurred,
                                     lineNumber);

            continue;
        }

        if (HasValidSymbol(sentence))
        {
            hasSymbolDefinition = TRUE;
        }

        if (IsDataSentence(sentence) || IsStringSentence(sentence))
        {
            if (hasSymbolDefinition)
            {
                InsertSymbolToSymbolTable(sentence,
                                          symbolTableHead,
                                          DATA,
                                          DC,
                                          &errorHasOccurred,
                                          lineNumber);
            }

            InsertToDataArray(dataArray,
                              sentence,
                              &DC,
                              *symbolTableHead,
                              &errorHasOccurred,
                              lineNumber);

            continue;
        }

        if (IsExternSentence(sentence))
        {
            hasExternals = TRUE;

            if (hasSymbolDefinition)
            {
                fprintf(stderr, "Warning: symbol definition at the start of extern instruction\n");
            }

            InsertExternToSymbolTable(sentence,
                                      symbolTableHead,
                                      &errorHasOccurred,
                                      lineNumber);

            continue;
        }

        if (IsEntrySentence(sentence))
        {
            hasEntries = TRUE;

            if (hasSymbolDefinition)
            {
                fprintf(stderr, "Warning: symbol definition at the start of entry instruction\n");
            }

            continue;
        }

        if (hasSymbolDefinition)
        {
            InsertSymbolToSymbolTable(sentence,
                                      symbolTableHead,
                                      CODE,
                                      IC + STARTING_ADDRESS,
                                      &errorHasOccurred,
                                      lineNumber);
        }

        if (!HasValidOperationName(sentence))
        {
            fprintf(stderr, "Line %d:\tError: unknown operation name\n", lineNumber);
            errorHasOccurred = TRUE;
        }
        else
        {
            BuildFirstMemoryWord(instructionsArray,
                                 sentence,
                                 &IC,
                                 *symbolTableHead,
                                 &errorHasOccurred,
                                 lineNumber);
        }

    } /* End of while */

    if (!errorHasOccurred)
    {
        UpdateDataSymbols(*symbolTableHead, IC + STARTING_ADDRESS);
        RunSecondScan(assemblyFile,
                      instructionsArray,
                      dataArray,
                      symbolTableHead,
                      filename,
                      hasEntries,
                      hasExternals,
                      DC);
    }
}

static void RunSecondScan(FILE *assemblyFile,
                          MemoryWord *instructionsArray,
                          MemoryWord *dataArray,
                          SymbolTableNode **symbolTableHead,
                          const char *filename,
                          bool hasEntries,
                          bool hasExternals,
                          int dataCounter)
{
    char sentence[MAX_SENTENCE_SIZE] = {0};
    int IC = 0, lineNumber = 0;
    bool errorHasOccurred = FALSE;

    /* Sets the file position indicator to the beginning of the file */
    rewind(assemblyFile);

    while (fgets(sentence, MAX_SENTENCE_SIZE, (FILE *)assemblyFile))
    {
        ++lineNumber;

        if (IsCommentSentence(sentence) ||
            IsEmptySentence(sentence) ||
            IsDataSentence(sentence) ||
            IsStringSentence(sentence) ||
            IsExternSentence(sentence) ||
            IsMacroSentence(sentence))
        {
            continue;
        }

        if (IsEntrySentence(sentence))
        {
            char param[MAX_SENTENCE_SIZE] = {0};

            GetInstructionParams(sentence, param);
            UpdateSymbolTypeToEntry(*symbolTableHead, param);

            continue;
        }

        BuildOtherMemoryWords(instructionsArray,
                              sentence,
                              &IC,
                              *symbolTableHead,
                              &errorHasOccurred,
                              lineNumber);
    } /* End of while */

    if (!errorHasOccurred)
    {
        BuildFiles(instructionsArray,
                   dataArray,
                   *symbolTableHead,
                   filename,
                   hasEntries,
                   hasExternals,
                   dataCounter,
                   IC);
    }
}
