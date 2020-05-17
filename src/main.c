/****************************************
* ASSEMBLER: main.c                     *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#include <stdio.h>  /* FILE, fprintf, fopen, fclose */
#include <errno.h>  /* errno */
#include <string.h> /* strerror, strcat, strcpy */
#include <stdlib.h> /* EXIT_SUCCESS */

#include "file_scanner.h"    /* API */
#include "assembler_utils.h" /* Utils file */

static const char *ASSEMBLY_FILE_POSTFIX = ".as";
static const char *READING_MODE = "r";

int main(int argc, char *argv[])
{
    int i = 0;

    for (i = 1; i < argc; ++i)
    {
        FILE *assemblyFile = NULL;
        char filename[MAX_FILENAME_SIZE] = {0};

        strcpy(filename, argv[i]);
        strcat(filename, ASSEMBLY_FILE_POSTFIX);

        assemblyFile = fopen(filename, READING_MODE);
        if (NULL == assemblyFile)
        {
            fprintf(stderr, "Error opening file \"%s\": %s\n", filename, strerror(errno));
            continue;
        }

        RunScans(assemblyFile, argv[i]);

        fclose(assemblyFile);
    }

    return EXIT_SUCCESS;
}
