/*
 * File: lexer.h
 * Author: John Powell
 * 
 * Contains the rules for tokenization
 */
#ifndef JEP_LEXER_H_
#define JEP_LEXER_H_

#include "scanner.h"

extern const char WHITESPACE[3];
extern const char DIGIT[10] ;
extern const char LETTER[52];

extern const char IDENTIFIER_START[54];

extern const char SYMBOLS_1[13][2];

extern const char SYMBOLS_2[12][3];

extern const char KEYWORD[4][6];

extern const char PRIMITIVE[6][7];

extern const char TOKEN_IDENTIFIER[11];
extern const char TOKEN_SYMBOL[7];
extern const char TOKEN_NUMBER[7];
extern const char TOKEN_STRING[7];

typedef struct Token
{
    char* type;
    char* value;
    int row;
    int column;
} token_t;

int Tokenize(jchar_t* source, token_t** token, int nmem);
int CreateToken(jchar_t* characters, token_t* token);
int IsWhitespace(char character);
int IsDigit(char character);
int IsLetter(char character);
int IsNumber(char* characters);
int IsString(char* characters);
int IsKeyword(char* characters);
int IsIdentifier(char* characters);
int IsSymbol(char* characters);

#endif
