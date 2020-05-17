/****************************************
* ASSEMBLER: sentence_analyzer.h        *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#ifndef ASSEMBLER_SENTENCE_ANALYZER_H
#define ASSEMBLER_SENTENCE_ANALYZER_H

#include "assembler_utils.h" /* Utils file */

bool IsCommentSentence(const char *sentence);
bool IsMacroSentence(const char *sentence);
bool IsEntrySentence(const char *sentence);
bool IsExternSentence(const char *sentence);
bool IsEmptySentence(const char *sentence);
bool IsDataSentence(const char *sentence);
bool IsStringSentence(const char *sentence);
bool HasValidSymbol(const char *sentence);
bool HasValidOperationName(const char *sentence);

void GetString(const char *stringSentence, char *string);
void GetData(const char *dataSentence, char *data);
void GetOperationName(const char *sentence, char *operationName);
void GetSymbol(const char *sentence, char *symbol);
void GetInstructionParams(const char *instructionSentence, char *param);

void RemoveWhiteSpaces(char *str);
int FindChar(const char *str, char c);

#endif /* ASSEMBLER_SENTENCE_ANALYZER_H */
