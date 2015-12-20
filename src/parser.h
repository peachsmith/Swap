#ifndef JEP_PARSER_H_
#define JEP_PARSER_H_

#include "lexer.h"

typedef struct TokenStream
{
	token_t* next;
	int syntax_error;
} tstream_t;


typedef struct Stack
{
	char** data;
	int capacity;
	int size;
} stack_t;

typedef struct StatementQueue
{
	token_t** data;
	int capacity;
	int size;
} squeue_t;

typedef struct Object
{
	char* identifier;
	char* type;
	char* value;
	int arg_count;
} object_t;

typedef struct ObjectStack
{
	object_t* objects;
	int capacity;
	int size;
} ostack_t;

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
char* Evaluate(token_t** token, stack_t* expressions, stack_t* operators, ostack_t* ostack);
char* ValidateSemantics(token_t** token, stack_t* expressions, stack_t* operators, ostack_t* ostack);
void Interpret(token_t* token, stack_t* expressions, stack_t* operators);
int CreateObject(ostack_t* ostack, char* identifier, char* type, char* value, int arg_count);
void ResizeObjectStack(ostack_t** ostack);
int Exists(ostack_t* ostack, char* identifier);
void PrintObjects(ostack_t* ostack);

void AddStatement(squeue_t* squeue, token_t* statement);
void Resize(squeue_t** squeue);

void NativeFunctionCall(const char* function_name, char** args);

#endif
