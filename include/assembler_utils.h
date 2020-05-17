/****************************************
* ASSEMBLER: assembler_utils.h          *
* 	                                    *
* Written by: Magal Horesh              *
* Date: 19/08/2019                      *
****************************************/

#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

#define MAX_SENTENCE_SIZE (100)
#define MAX_FILENAME_SIZE (100)
#define MAX_LABEL_SIZE (31)

static const char COMMENT_SENTENCE_PREFIX[] = ";";
static const char MACRO_SENTENCE_PREFIX[] = ".define";
static const char DATA_SENTENCE_PREFIX[] = ".data";
static const char STRING_SENTENCE_PREFIX[] = ".string";
static const char ENTRY_SENTENCE_PREFIX[] = ".entry";
static const char EXTERN_SENTENCE_PREFIX[] = ".extern";
static const char COMMA_SIGN_ARRAY[] = ",";

static const char NEW_LINE = '\n';
static const char END_LINE = '\0';
static const char EQUAL_SIGN = '=';
static const char COLON_SIGN = ':';
static const char COMMA_SIGN = ',';
static const char QUOTATION_MARK_SIGN = '"';
static const char PLUS_SIGN = '+';
static const char MINUS_SIGN = '-';
static const char REGISTER_PREFIX = 'r';
static const char ZERO_DIGIT = '0';
static const char NINE_DIGIT = '9';
static const char OPENING_SQUARE_BRACKETS = '[';
static const char CLOSING_SQUARE_BRACKETS = ']';
static const char HASH_MARK = '#';

static const int NOT_FOUND = -2;
static const int STARTING_ADDRESS = 100;

typedef enum
{
    FALSE = 0,
    TRUE = 1
} bool;

typedef enum
{
    ERROR = -1,
    SUCCESS = 0,
    FAILURE = 1
} ReturnStatus;

#endif /* ASSEMBLER_UTILS_H */
