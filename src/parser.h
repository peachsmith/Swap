#ifndef JEP_PARSER_H_
#define JEP_PARSER_H_

#include "lexer.h"

typedef struct TokenStream
{
	token_t* next;
	int indent;
	int syntax_error;
} tstream_t;

void NextToken(tstream_t* stream);
int Accept(const char* expected, tstream_t* stream);
int Expect(const char* expected, tstream_t* stream);
void Factor(tstream_t* stream);
void Term(tstream_t* stream);
void Expression(tstream_t* stream);
void Statement(tstream_t* stream);
void Block(tstream_t* stream);

#endif
