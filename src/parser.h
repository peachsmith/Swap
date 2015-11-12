#ifndef JEP_PARSER_H_
#define JEP_PARSER_H_

#include "lexer.h"

typedef enum NodeType 
{
	ROOT, INTEGER, OPERATOR, CONSTANT, MISC
} ntype_t;

typedef struct AbstractSyntaxNode
{
	char* value;
	ntype_t type;
	int child_count;
	struct AbstractSyntaxNode* child_nodes;
} ast_node_t;

typedef struct TokenStream
{
	token_t* next;
	int indent;
	int syntax_error;
	ast_node_t* ast_node;
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
