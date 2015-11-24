#include "parser.h"

void NextToken(tstream_t* stream)
{
	stream->next++;
}

int Accept(const char* expected, tstream_t* stream)
{
	if(!strcmp(expected, "number") 
		|| !strcmp(expected, "identifier")
		|| !strcmp(expected, "keyword"))
	{
		if(!strcmp(stream->next->type, expected))
		{
			NextToken(stream);
			return 1;
		}
		else
			return 0;
	}

	if(!strcmp(stream->next->value, expected))
	{
		NextToken(stream);
		return 1;
	}
	else 
		return 0;
}

int Expect(const char* expected, tstream_t* stream)
{
	if(Accept(expected,stream))
		return 1;
	else
	{
		if(!stream->syntax_error)
		{
			printf("\nunexpected token row: %d column: %d. expected: %s  found %s\n", stream->next->row, stream->next->column, expected, stream->next->value);
			stream->syntax_error = 1;
		}
	}
	return 0;
}

void Factor(tstream_t* stream)
{
	if(Accept("identifier",stream))
	{

	}
	else if(Accept("number",stream))
	{

	}
	else if(Accept("(",stream))
	{
		Expression(stream);
		Expect(")",stream);
	}
	else
	{
		if(!stream->syntax_error)
		{
			printf("\nsyntax error\n");
			stream->syntax_error = 2;
		}
		NextToken(stream);
	}
}

void Term(tstream_t* stream)
{
	Factor(stream);
	while (Accept("*",stream) || Accept("/",stream))
		Factor(stream);
}

void Expression(tstream_t* stream)
{
	if(Accept("+",stream) || Accept("-",stream));
	Term(stream);
	while(Accept("+",stream) || Accept("-",stream))
		Term(stream);

	/* assignment */
	if(Accept("=",stream) || Accept("==",stream) || Accept("!=",stream)
		|| Accept("<",stream) || Accept(">",stream) || Accept("<=",stream)
		|| Accept(">=",stream))
		Expression(stream);
}

void Statement(tstream_t* stream)
{
	stream->indent += 2;
	if(Accept(";",stream))
	{
		
	}
	else if(Accept("if",stream))
	{
		Expect("(",stream);
		Expression(stream);
		Expect(")",stream);
		Block(stream);
	}
	else
	{
		Expression(stream);
		Expect(";",stream);
	}
	stream->indent -= 2;
}

void Block(tstream_t* stream)
{
	if(Accept("{",stream))
	{
		if(!Accept("}",stream))
		{
			while(strcmp(stream->next->value, "}") && strcmp(stream->next->value, "end of stream"))
				Statement(stream);
			Expect("}",stream);

		}
	}
	else
	{
		Statement(stream);
	}
}

void Push(stack_t* stack, char* value)
{
	int value_size = 0;
	while(value[value_size] != '\0')
		value_size++;

	if(stack->size < stack->capacity)
	{
		stack->data[stack->size] = malloc(sizeof(char) * (value_size + 1));
		int i;
		for(i = 0; i < value_size; i++)
			stack->data[stack->size][i] = value[i];
		stack->data[stack->size++][i] = '\0';
	}
	else
	{
		char** temp_data = malloc(sizeof(char*) * stack->capacity);
		int i;
		for(i = 0; i < stack->capacity; i++)
			temp_data[i] = stack->data[i];

		free(stack->data);
		stack->data = malloc(sizeof(char*) * (stack->capacity + stack->capacity / 2));
		
		for(i = 0; i < stack->capacity; i++)
			stack->data[i] = temp_data[i];

		free(temp_data);
		stack->data[stack->size] = malloc(sizeof(char) * (value_size + 1));

		for(i = 0; i < value_size; i++)
			stack->data[stack->size][i] = value[i];
		stack->data[stack->size++][i] = '\0';
		stack->capacity = stack->capacity + stack->capacity / 2;
	}
}

void Pop(stack_t* stack, char** value)
{
	if(stack->size)
		*value = stack->data[stack->size-- - 1];
}

void PopAll(stack_t* stack)
{
	int i;
	for(i = 0; i < stack->size; i++)
		free(stack->data[i]);
	free(stack->data);
	stack->size = 0;
	stack->capacity = 10;
	stack->data = malloc(sizeof(char*) * stack->capacity);
}

void PrintStack(stack_t* stack)
{
	int i;
	for(i = 0; i < stack->size; i++)
		printf("%s, ", stack->data[i]);
	printf("\n");
}

int Priority(char* value)
{
	if(!strcmp(value, "+") || !strcmp(value, "-"))
		return 1;
	else if(!strcmp(value, "*") || !strcmp(value, "/"))
		return 2;
	else if(!strcmp(value, "(") || !strcmp(value, ")") || !strcmp(value, "="))
		return 4;
	else return 0;
}

void EvaluateBinaryOperation(char** opr,  char** l_operand, char** r_operand, char** result)
{
	char l_buffer[30];
	char r_buffer[30];
	char result_buffer[30];
	int l_op;
	int r_op; 
	sscanf(*l_operand, "%d", &l_op);
	sscanf(*r_operand, "%d", &r_op);
	int i = 0;

	if(!strcmp(*opr, "+"))
	{
		sprintf(result_buffer, "%d", l_op + r_op);
		*result = malloc(sizeof(char) * 30);
		while(result_buffer[i] != '\0')
			(*result)[i] = result_buffer[i++];
		(*result)[i] = result_buffer[i];
	}
	else if(!strcmp(*opr, "-"))
	{
		sprintf(result_buffer, "%d", l_op - r_op);
		*result = malloc(sizeof(char) * 30);
		while(result_buffer[i] != '\0')
			(*result)[i] = result_buffer[i++];
		(*result)[i] = result_buffer[i];
	}
	else if(!strcmp(*opr, "*"))
	{
		sprintf(result_buffer, "%d", l_op * r_op);
		*result = malloc(sizeof(char) * 30);
		while(result_buffer[i] != '\0')
			(*result)[i] = result_buffer[i++];
		(*result)[i] = result_buffer[i];
	}
	else if(!strcmp(*opr, "/"))
	{
		sprintf(result_buffer, "%d", l_op / r_op);
		*result = malloc(sizeof(char) * 30);
		while(result_buffer[i] != '\0')
			(*result)[i] = result_buffer[i++];
		(*result)[i] = result_buffer[i];
	}
}

char* Evaluate(token_t** token, stack_t* expressions, stack_t* operators, ostack_t* ostack)
{
	while(strcmp((*token)->value, "end of stream"))
	{
		if(!strcmp((*token)->type, "number"))
		{
			Push(expressions, (*token)->value);
		}
		else if(!strcmp((*token)->type, "identifier"))
		{
			char* identifier = (*token)->value;
			char* type;
			if(strcmp((*token + 1)->value, "end of stream"))
			{
				if(!expressions->size && !operators->size)
				{
					if(!strcmp((*token + 1)->value, ";"))
					{
						CreateObject(ostack, identifier, "null", "null");
					}	
					else if(!strcmp((*token + 1)->value, "="))
					{
						(*token)++;
						if(strcmp((*token + 1)->value, "end of stream") && strcmp((*token + 1)->value, ";"))
							(*token)++;
						else
						{
							printf("incomplete assignment. expected expression, found %s\n", (*token + 1)->value);
							return 0;
						}
						char* result = Evaluate(token, expressions, operators, ostack);
						if(result)
						{
							if(result[0] != '\0' && result[0] == '"')
								type = "string";
							else
								type = "number";

							CreateObject(ostack, identifier, type, result);
							if(!strcmp((*token)->value,"end of stream"))
							{
								PopAll(expressions);
								PopAll(operators);
								return 0;
							}
							else
							{
								PopAll(expressions);
								PopAll(operators);
								continue;
							}
						}
					}
					else
					{
						int object_index = Exists(ostack, identifier);
						if(object_index > -1)
						{
							Push(expressions, ostack->objects[object_index].value);
						}
						else
						{
							printf("no object with identifier '%s' has been defined.\n", identifier);
							return 0;
						}
					}	
				}
			}
		}
		else if(!strcmp((*token)->type, "symbol")
			&& strcmp((*token)->value, "{") 
			&& strcmp((*token)->value, "}")
			&& strcmp((*token)->value, "=")
			&& strcmp((*token)->value, ";"))
		{
			if(operators->size)
			{
				if(Priority((*token)->value) <= Priority(operators->data[operators->size - 1]))
				{
					while(operators->size > 0 && strcmp(operators->data[operators->size - 1], "("))
					{
						char* l_op;
						char* r_op;
						char* opr;
						char* result; // malloc
						Pop(expressions, &r_op);
						Pop(expressions, &l_op);
						Pop(operators, &opr);

						EvaluateBinaryOperation(&opr, &l_op, &r_op, &result);
									
						free(expressions->data[expressions->size + 1]);
						free(expressions->data[expressions->size]);
						free(operators->data[operators->size]);

						Push(expressions, result);
						free(result);
					}
								
					Push(operators, (*token)->value);
				}
				else if(!strcmp((*token)->value, ")"))
				{
					while(operators->size > 0 && strcmp(operators->data[operators->size - 1], "("))
					{
						char* l_op;
						char* r_op;
						char* opr;
						char* result; // malloc
						Pop(expressions, &r_op);
						Pop(expressions, &l_op);
						Pop(operators, &opr);

						EvaluateBinaryOperation(&opr, &l_op, &r_op, &result);
									
						free(expressions->data[expressions->size + 1]);
						free(expressions->data[expressions->size]);
						free(operators->data[operators->size]);

						Push(expressions, result);
						free(result);
					}
					if(!operators->size)
					{
						printf("mismatched parentheses\n");
						break;
					}
					if(!strcmp(operators->data[operators->size - 1], "("))
					{
						char* l_paren;
						Pop(operators, &l_paren);
						free(operators->data[operators->size]);
					}
					else
						printf("mismatched parentheses\n");
				}
				else
					Push(operators, (*token)->value);
			}
			else
				Push(operators, (*token)->value);
		}
		else if(!strcmp((*token)->value, ";"))
		{
			while(operators->size > 0)
			{
				char* l_op;
				char* r_op;
				char* opr;
				char* result; // malloc
				Pop(expressions, &r_op);
				Pop(expressions, &l_op);
				Pop(operators, &opr);

				EvaluateBinaryOperation(&opr, &l_op, &r_op, &result);

				free(expressions->data[expressions->size + 1]);
				free(expressions->data[expressions->size]);
				free(operators->data[operators->size]);

				Push(expressions, result);
				free(result);
			}
			PopAll(operators);
			if(expressions->size > 1)
			{
				PopAll(expressions);
				printf("could not evalute all expressions\n");
				return 0;
			}
			else if(expressions->size == 1)
			{
				(*token)++;
				char* result;
				Pop(expressions, &result);
				return result;
			}
		}
		(*token)++;
	}
	
	return 0;
}

void Interpret(token_t* token, stack_t* expressions, stack_t* operators)
{	
	ostack_t ostack;
	ostack.size = 0;
	ostack.capacity = 10;
	ostack.objects = malloc(sizeof(object_t) * 10);

	while(strcmp(token->value, "end of stream"))
	{
		char* result = Evaluate(&token, expressions, operators, &ostack);
		if(result)
		{
			printf("result: %s\n", result);
			free(result);
		}
		else
		{
			break;
		}
	}

	int i;
	for(i = 0; i < ostack.size; i++)
	{
		free(ostack.objects[i].identifier);
		free(ostack.objects[i].type);
		free(ostack.objects[i].value);
	}
	free(ostack.objects);
}

int CreateObject(ostack_t* ostack, char* identifier, char* type, char* value)
{
	int i;
	int identifier_size = 0;
	int type_size = 0;
	int value_size = 0;

	while(identifier[identifier_size] != '\0')
		identifier_size++;
	identifier_size++;

	while(type[type_size] != '\0')
		type_size++;
	type_size++;

	while(value[value_size] != '\0')
		value_size++;
	value_size++;

	if(ostack->size >= ostack->capacity)
	{
		printf("cannot create any more objects. %d objects already exist.\n", ostack->size);
		return 0;
	}
	else if(Exists(ostack, identifier) == -1)
	{
		ostack->objects[ostack->size].identifier = malloc(sizeof(char) * identifier_size);
		ostack->objects[ostack->size].type = malloc(sizeof(char) * type_size);
		ostack->objects[ostack->size].value = malloc(sizeof(char) * value_size);

		for(i = 0; i < identifier_size; i++)
			ostack->objects[ostack->size].identifier[i] = identifier[i];

		for(i = 0; i < type_size; i++)
			ostack->objects[ostack->size].type[i] = type[i];

		for(i = 0; i < value_size; i++)
			ostack->objects[ostack->size].value[i] = value[i];

		ostack->size++;

		printf("created an object: %s \n%d existing object(s):\n", identifier, ostack->size);
		PrintObjects(ostack);
	}
	else
	{
		printf("object %s already exists.\n", identifier);
		return 0;
	}
}

int Exists(ostack_t* ostack, char* identifier)
{
	int i;
	for(i = 0; i < ostack->size; i++)
	{
		if(!strcmp(ostack->objects[i].identifier, identifier))
			return i;
	}
	return -1;
}

void PrintObjects(ostack_t* ostack)
{
	int i;
	for(i = 0; i < ostack->size; i++)
		printf("  identifier: %s, type: %s, value: %s\n", 
			ostack->objects[i].identifier, ostack->objects[i].type, ostack->objects[i].value);
}
