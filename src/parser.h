#ifndef JEP_PARSER_H_
#define JEP_PARSER_H_

#include "lexer.h"

typedef struct TokenStream
{
	token_t* next;
	int indent;
	int syntax_error;
} tstream_t;

typedef struct Stack
{
	char** data;
	int capacity;
	int size;
} stack_t;

void NextToken(tstream_t* stream);
int Accept(const char* expected, tstream_t* stream);
int Expect(const char* expected, tstream_t* stream);
void Factor(tstream_t* stream);
void Term(tstream_t* stream);
void Expression(tstream_t* stream);
void Statement(tstream_t* stream);
void Block(tstream_t* stream);

void Push(stack_t* stack, char* value);
void Pop(stack_t* stack, char** value);
void PopAll(stack_t* stack);
int Priority(char* token);
void PrintStack(stack_t* stack);
void EvaluateBinaryOperation(char** opr, char** l_operand, char** r_operand, char** result);
char* Evaluate(token_t** token, stack_t* expressions, stack_t* operators);
void Interpret(token_t* token, stack_t* expressions, stack_t* operators);

#endif
