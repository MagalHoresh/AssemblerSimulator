/****************************************
* ASSEMBLER: files_builder.c            *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#include <stdio.h>  /* FILE, fprintf, fopen, fclose */
#include <errno.h>  /* errno */
#include <string.h> /* strerror, strcat, strcpy */
#include <assert.h> /* assert */

#include "files_builder.h"   /* API */
#include "assembler_utils.h" /* Utils file */

#define PART_SIZE_IN_BITS (2)

typedef struct
{
    unsigned int part1 : PART_SIZE_IN_BITS;
    unsigned int part2 : PART_SIZE_IN_BITS;
    unsigned int part3 : PART_SIZE_IN_BITS;
    unsigned int part4 : PART_SIZE_IN_BITS;
    unsigned int part5 : PART_SIZE_IN_BITS;
    unsigned int part6 : PART_SIZE_IN_BITS;
    unsigned int part7 : PART_SIZE_IN_BITS;
} SpecialWord;

static const char *OBJECT_FILE_POSTFIX = ".ob";
static const char *ENTRY_FILE_POSTFIX = ".ent";
static const char *EXTERN_FILE_POSTFIX = ".ext";
static const char *WRITING_MODE = "w";

static unsigned int CONVERTER_LUT[4] = {'*', '#', '%', '!'};

static void BuildObjectFile(MemoryWord *instructionsArray,
                            MemoryWord *dataArray,
                            int dataCounter,
                            int instructionCounter,
                            const char *filename);
static void BuildEntriesFile(SymbolTableNode *symbolTableHead,
                             const char *filename);
static void BuildExternalsFile(SymbolTableNode *symbolTableHead,
                               const char *filename);
static void WriteToObjectFile(FILE *objectFile,
                              MemoryWord *instructionsArray,
                              MemoryWord *dataArray,
                              int dataCounter,
                              int instructionCounter);
static void WriteSpecialWordToObjectFile(FILE *objectFile,
                                         SpecialWord *specialWord);
static FILE *OpenFile(const char *filename, const char *postfix);
static void CloseFile(FILE *file);

void BuildFiles(MemoryWord *instructionsArray,
                MemoryWord *dataArray,
                SymbolTableNode *symbolTableHead,
                const char *filename,
                bool hasEntries,
                bool hasExternals,
                int dataCounter,
                int instructionCounter)
{
    assert(NULL != instructionsArray);
    assert(NULL != dataArray);
    assert(NULL != symbolTableHead);
    assert(NULL != filename);
    assert(dataCounter >= 0);
    assert(instructionCounter >= 0);

    BuildObjectFile(instructionsArray,
                    dataArray,
                    dataCounter,
                    instructionCounter,
                    filename);

    if (hasEntries)
    {
        BuildEntriesFile(symbolTableHead, filename);
    }

    if (hasExternals)
    {
        BuildExternalsFile(symbolTableHead, filename);
    }
}

/* Static functions */
static void BuildObjectFile(MemoryWord *instructionsArray,
                            MemoryWord *dataArray,
                            int dataCounter,
                            int instructionCounter,
                            const char *filename)
{
    FILE *objectFile = OpenFile(filename, OBJECT_FILE_POSTFIX);

    if (NULL != objectFile)
    {
        WriteToObjectFile(objectFile,
                          instructionsArray,
                          dataArray,
                          dataCounter,
                          instructionCounter);
        CloseFile(objectFile);
    }
}

static void BuildEntriesFile(SymbolTableNode *symbolTableHead,
                             const char *filename)
{
    FILE *entriesFile = OpenFile(filename, ENTRY_FILE_POSTFIX);

    if (NULL != entriesFile)
    {
        WriteToFileByType(entriesFile, symbolTableHead, ENTRY);
        CloseFile(entriesFile);
    }
}

static void BuildExternalsFile(SymbolTableNode *symbolTableHead,
                               const char *filename)
{
    FILE *externalsFile = OpenFile(filename, EXTERN_FILE_POSTFIX);

    if (NULL != externalsFile)
    {
        WriteToFileByType(externalsFile, symbolTableHead, EXTERNAL);
        CloseFile(externalsFile);
    }
}

static void WriteToObjectFile(FILE *objectFile,
                              MemoryWord *instructionsArray,
                              MemoryWord *dataArray,
                              int dataCounter,
                              int instructionCounter)
{
    int i = 0;
    fprintf(objectFile, "\t%d %d\n", instructionCounter, dataCounter);

    for (i = 0; i < instructionCounter; ++i)
    {
        SpecialWord *specialWord = (SpecialWord *)(instructionsArray + i);

        fprintf(objectFile, "%04d\t", STARTING_ADDRESS + i);
        WriteSpecialWordToObjectFile(objectFile, specialWord);
    }

    for (i = 0; i < dataCounter; ++i)
    {
        SpecialWord *specialWord = (SpecialWord *)(dataArray + i);

        fprintf(objectFile, "%04d\t", STARTING_ADDRESS + instructionCounter + i);
        WriteSpecialWordToObjectFile(objectFile, specialWord);
    }
}

static void WriteSpecialWordToObjectFile(FILE *objectFile,
                                         SpecialWord *specialWord)
{
    fprintf(objectFile, "%c", CONVERTER_LUT[specialWord->part7]);
    fprintf(objectFile, "%c", CONVERTER_LUT[specialWord->part6]);
    fprintf(objectFile, "%c", CONVERTER_LUT[specialWord->part5]);
    fprintf(objectFile, "%c", CONVERTER_LUT[specialWord->part4]);
    fprintf(objectFile, "%c", CONVERTER_LUT[specialWord->part3]);
    fprintf(objectFile, "%c", CONVERTER_LUT[specialWord->part2]);
    fprintf(objectFile, "%c\n", CONVERTER_LUT[specialWord->part1]);
}

static FILE *OpenFile(const char *filename, const char *postfix)
{
    FILE *file = NULL;
    char filenameWithPostfix[MAX_FILENAME_SIZE] = {0};

    strcpy(filenameWithPostfix, filename);
    strcat(filenameWithPostfix, postfix);

    file = fopen(filenameWithPostfix, WRITING_MODE);
    if (NULL == file)
    {
        fprintf(stderr, "Error opening file \"%s\": %s\n", filename, strerror(errno));
        return NULL;
    }

    return file;
}

static void CloseFile(FILE *file)
{
    fclose(file);
}
