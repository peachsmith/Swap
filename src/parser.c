#include "parser.h"
#include "swapstd.h"

void NextToken(tstream_t* stream)
{
	stream->next++;
}

int Accept(const char* expected, tstream_t* stream)
{
	if(!strcmp(expected, "number") 
		|| !strcmp(expected, "identifier")
		|| !strcmp(expected, "keyword")
		|| !strcmp(expected, "string"))
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
	if(Accept("identifier", stream))
	{
		if(Accept("(", stream))
		{
			if(Accept(")", stream))
			{
				// empty parentheses
			}
			else
			{
				Expression(stream);
				Expect(")", stream);
			}
		}
	}
	else if(Accept("number", stream))
	{

	}
	else if(Accept("(", stream))
	{
		Expression(stream);
		Expect(")", stream);
	}
	else if(Accept("string", stream))
	{
		while(Accept("+", stream))
			Expression(stream);
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
	if(Accept("+", stream) || Accept("-", stream));
	Term(stream);
	while(Accept("+", stream) || Accept("-", stream))
		Term(stream);

	// this stuff
	if(Accept("=", stream) || Accept("==", stream) || Accept("!=", stream)
		|| Accept("<", stream) || Accept(">", stream) || Accept("<=", stream)
		|| Accept(">=", stream))
		Expression(stream);

	if(Accept(",", stream))
		Expression(stream);
}

void Statement(tstream_t* stream)
{
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
	int l_size;
	int r_size;
	sscanf(*l_operand, "%d", &l_op);
	sscanf(*r_operand, "%d", &r_op);
	int i = 0;

	if(!strcmp(*opr, "+"))
	{
		if(*l_operand[0] == '"' || *r_operand[0] == '"')
		{
			// string concatenation
			l_size = length(*l_operand);			
			r_size = length(*r_operand);
			int concat_size = l_size + r_size;

			*result = malloc(sizeof(char) * concat_size + sizeof(char) * 4);
			//printf("l_size: %d, r_size: %d, concat_size: %d\n", l_size, r_size, concat_size);

			// add the opening quotes
			(*result)[0] = '"';

			//int quote_index = 0;

			for(i = 0; i < l_size - 1; i++)
			{
				if(i == 0 && (*l_operand)[i] == '"')
				{
					//quote_index++;
					continue;
				}
				else if(i == l_size - 2 && (*l_operand)[i] == '"')
				{
					break;
				}
				else
				{
					//printf("i: %d, (*l_operand)[i]: %c\n", i, (*l_operand)[i]);
					(*result)[i] = (*l_operand)[i];
				}
			}
			
			//printf("i = %d\n", i);
			
			int quote_index = 0;

			for(; i < l_size + r_size - 1; i++)
			{
				//printf("i: %d, c: %c\n", i, (*r_operand)[i - l_size]);
				if((i - l_size == 0) && (*r_operand)[i - l_size] == '"')
				{
					quote_index++;
					continue;
				}
				else if((i - l_size == r_size - 2) && (*r_operand)[i - l_size] == '"')
				{
					break;
				}
				else if(i - l_size >= 0)
				{
					//printf("i: %d, (*r_operand)[%d - %d]): %c\n", i, i, l_size, (*r_operand)[i - l_size]);
					(*result)[i - quote_index] = (*r_operand)[i - l_size];
				}
				else
				{
					quote_index++;
				}
			}
			//printf("\n----\n");
			//printf("i = %d\n", i);
			(*result)[i - quote_index] = '"';
			(*result)[i - quote_index + 1] = '\0';
			//printf("concatenation result: %s\n", *result);
		}
		else
		{
			//printf("encountered addition\n");
			//printf("attempting to add %s with %s\n", *l_operand, *r_operand);
			sprintf(result_buffer, "%d", l_op + r_op);
			*result = malloc(sizeof(char) * 30);
			while(result_buffer[i] != '\0')
				(*result)[i] = result_buffer[i++];
			(*result)[i] = result_buffer[i];
		}
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
	int parentheses = 0;
	while(strcmp((*token)->value, "end of stream"))
	{
		if(!strcmp((*token)->type, "number") || !strcmp((*token)->type, "string"))
		{
			Push(expressions, (*token)->value);
		}
		else if(!strcmp((*token)->type, "identifier"))
		{
			char* identifier = (*token)->value;
			char* type;
			token_t* loc = *token;
			if(strcmp((*token + 1)->value, "end of stream"))
			{
				if(!expressions->size && !operators->size)
				{
					if(!strcmp((*token + 1)->value, ";")) // declaration
					{
						CreateObject(ostack, identifier, "null", "null", 0);
					}	
					else if(!strcmp((*token + 1)->value, "=")) // assignment
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

							int obj_index = Exists(ostack, identifier);
							if(obj_index > -1)
								ostack->objects[obj_index].value = result;
							else
								CreateObject(ostack, identifier, type, result, 0);
							
							if(!strcmp((*token)->value,"end of stream"))
							{
								PopAll(expressions);
								PopAll(operators);
								free(result);
								return 0;
							}
							else
							{
								free(result);
								PopAll(expressions);
								PopAll(operators);
								continue;
							}
						}
						else
						{
							return 0;
						}
					}
					else if(!strcmp((*token + 1)->value, "(")) // function call
					{
						int func_index = Exists(ostack, (*token)->value);
						if(func_index == -1)
						{
							printf("undefined identifier '%s' line %d, column %d\n", loc->value,
								loc->row, loc->column);
							return 0;
						}
						else if(!strcmp(ostack->objects[func_index].type, "native function"))
						{
							(*token)++;
							(*token)++;
							
							int i;
							int size = 0;
							int capacity = 5;
							char** args = malloc(sizeof(char*) * capacity);
							while(strcmp((*token)->value, ")"))
							{
								char* result;
								result = Evaluate(token, expressions, operators, ostack);
								
								if(result)
								{
									//write(result);
									//return result;
									//free(result);
									if(size == capacity)
									{
										capacity = capacity + capacity / 2;
										char** new_args = malloc(sizeof(char*) * capacity);
										for(i = 0; i < size; i++)
											new_args[i] = args[i];
										free(args);
										args = new_args;
									}
									args[size++] = result;

									if(!strcmp((*token)->value, ","))
										(*token)++;
								}
								else
								{
									for(i = 0; i < size; i++)
										free(args[i]);
									free(args);
									return 0;
								}
							}
							(*token)++;
							if(size != ostack->objects[func_index].arg_count)
							{
								printf("incorrect number of arguments for function %s. line %d column %d\n", 
									ostack->objects[func_index].identifier, loc->row, loc->column);
								printf("expected %d found %d\n", ostack->objects[func_index].arg_count, size);
								for(i = 0; i < size; i++)
									free(args[i]);
								free(args);
								return 0;
							}
							else
							{
								NativeFunctionCall(ostack->objects[func_index].identifier, args);
								for(i = 0; i < size; i++)
									free(args[i]);
								free(args);
								char* result = malloc(sizeof(char));
								return result;
							}
						}
						else if(!strcmp(ostack->objects[func_index].type, "function"))
						{
							// swap function call
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
							printf("undefined identifier '%s' line %d, column %d.\n", identifier, 
								loc->row, loc->column);
							return 0;
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
						printf("undefined identifier '%s' line %d column %d.\n", identifier,
							loc->row, loc->column);
						return 0;
					}
				}
			}
		}
		else if(!strcmp((*token)->type, "symbol")
			&& strcmp((*token)->value, "{") 
			&& strcmp((*token)->value, "}")
			&& strcmp((*token)->value, "=")
			&& strcmp((*token)->value, ";")
			&& strcmp((*token)->value, ","))
		{
			if(!strcmp((*token)->value, "("))
				parentheses++;
			else if(!strcmp((*token)->value, ")"))
				parentheses--;

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
					if(!strcmp((*token)->value, ")"))
					{
						// I think something was probably supposed to happen here
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
					if(!operators->size && parentheses > 0)
					{
						printf("mismatched parentheses\n");
						break;
					}
					else if(!operators->size && parentheses < 0) // function call
					{
						if(expressions->size == 1)
					{
						char* result; // malloc
						Pop(expressions, &result);
						return result;
					}
					else if(expressions->size > 1)
					{
						printf("could not evaluate expression\n");
						return 0;
					}
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
			{
				if(parentheses < 0) // end of function call
				{
					if(expressions->size == 1)
					{
						char* result; // malloc
						Pop(expressions, &result);
						return result;
					}
					else if(expressions->size > 1)
					{
						printf("could not evaluate expression\n");
						return 0;
					}
					else if(!expressions->size)
					{
						printf("mismatched parentheses\n");
						return 0;
					}
				}
				Push(operators, (*token)->value);
			}
		}
		else if(!strcmp((*token)->value, ";") || !strcmp((*token)->value, ","))
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
				printf("could not evaluate all expressions\n");
				return 0;
			}
			else if(expressions->size == 1)
			{
				char* result;
				Pop(expressions, &result);
				return result;
			}
			else if(expressions->size == 0)
			{
				char* result = malloc(sizeof(char) * 2);
				return result;
			}
		}
		(*token)++;
	}
	
	return 0;
}

char* ValidateSemantics(token_t** token, stack_t* expressions, stack_t* operators, ostack_t* ostack)
{
	int parentheses = 0;
	while(strcmp((*token)->value, "end of stream"))
	{
		if(!strcmp((*token)->type, "number") || !strcmp((*token)->type, "string"))
		{
			Push(expressions, (*token)->value);
		}
		else if(!strcmp((*token)->type, "identifier"))
		{
			char* identifier = (*token)->value;
			char* type;
			token_t* loc = *token;
			if(strcmp((*token + 1)->value, "end of stream"))
			{
				if(!strcmp((*token + 1)->value, ";")) // declaration
				{
					CreateObject(ostack, identifier, "null", "null", 0);
				}	
				else if(!strcmp((*token + 1)->value, "=")) // assignment
				{
					(*token)++;
					if(strcmp((*token + 1)->value, "end of stream") && strcmp((*token + 1)->value, ";"))
						(*token)++;
					else
					{
						printf("incomplete assignment. expected expression, found %s\n", (*token + 1)->value);
						return 0;
					}
					char* result = ValidateSemantics(token, expressions, operators, ostack);
					if(result)
					{
						if(result[0] != '\0' && result[0] == '"')
							type = "string";
						else
							type = "number";

						int obj_index = Exists(ostack, identifier);
						if(obj_index > -1)
							ostack->objects[obj_index].value = result;
						else
							CreateObject(ostack, identifier, type, result, 0);
						
						if(!strcmp((*token)->value,"end of stream"))
						{
							free(result);
							PopAll(expressions);
							PopAll(operators);
							return 0;
						}
						else
						{
							free(result);
							PopAll(expressions);
							PopAll(operators);
							continue;
						}
					}
					else
					{
						return 0;
					}
				}
				else if(!strcmp((*token + 1)->value, "(")) // function call
				{
					int func_index = Exists(ostack, (*token)->value);
					if(func_index == -1)
					{
						printf("undefined identifier '%s' line %d, column %d\n", loc->value,
							loc->row, loc->column);
						return 0;
					}
					else if(!strcmp(ostack->objects[func_index].type, "native function"))
					{
						// native function call
						
						(*token)++;
						(*token)++;

						int arg_count = 0;
						
						// validate the arguments
						while(strcmp((*token)->value, ")"))
						{
							char* result;
							result = ValidateSemantics(token, expressions, operators, ostack);
							
							if(result)
							{
								free(result);
								arg_count++;
								if(!strcmp((*token)->value, ","))
									(*token)++;
							}
							else
								return 0;
						}
						(*token)++;
						if(arg_count != ostack->objects[func_index].arg_count)
						{
							printf("incorrect number of arguments for function %s. line %d column %d\n", 
								ostack->objects[func_index].identifier, loc->row, loc->column);
							printf("expected %d found %d\n", ostack->objects[func_index].arg_count, arg_count);
							return 0;
						}
						else
						{
							char* result = malloc(sizeof(char));
							return result;
						}
					}
					else if(!strcmp(ostack->objects[func_index].type, "function"))
					{
						// swap function call
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
						printf("undefined identifier '%s' line %d, column %d.\n", identifier, 
							loc->row, loc->column);
						return 0;
					}
				}
			}
			else
			{
				printf("end of stream\n");
			}
		}
		else if(!strcmp((*token)->type, "symbol")
			&& strcmp((*token)->value, "{") 
			&& strcmp((*token)->value, "}")
			&& strcmp((*token)->value, "=")
			&& strcmp((*token)->value, ";")
			&& strcmp((*token)->value, ","))
		{
			if(!strcmp((*token)->value, "("))
				parentheses++;
			else if(!strcmp((*token)->value, ")"))
				parentheses--;

			if(parentheses < 0) // end of function call
			{
				char* result = malloc(sizeof(char) * 2); // malloc
				return result;
			}
			Push(operators, (*token)->value);
		}
		else if(!strcmp((*token)->value, ";") || !strcmp((*token)->value, ","))
		{

			if(operators->size >= 1)
				PopAll(operators);

			if(expressions->size >= 1)
				PopAll(expressions);

			char* result = malloc(sizeof(char) * 2);
			return result;
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
	ostack.objects = malloc(sizeof(object_t) * ostack.capacity);

	// create the native function(s)
	CreateObject(&ostack, "write", "native function", "null", 1);

	// the amount of predefined functions
	int predefined = 1;

	squeue_t squeue;
	squeue.size = 0;
	squeue.capacity = 10;
	squeue.data = malloc(sizeof(token_t*) * squeue.capacity);

	// gather the tokens into a statement queue
	int i;
	int j;
	int error = 0;
	while(strcmp(token->value, "end of stream"))
	{
		int size = 0;
		int capacity = 10;
		token_t* statement = malloc(sizeof(token_t) * capacity);
		while(strcmp(token->value, "end of stream"))
		{
			if(size == capacity)
			{
				int i;
				token_t* resized_statement = malloc(sizeof(token_t) * (capacity + capacity / 2));
				for(i = 0; i < size; i++)
					resized_statement[i] = statement[i];
				free(statement);
				statement = resized_statement;
				capacity = capacity + capacity / 2;
			}

			statement[size++] = *token;

			if(!strcmp(token->value, ";"))
			{
				break;
			}
			token++;
		}

		AddStatement(&squeue, statement);

		token++;
	}

	// check semantics
	for(i = 0; i < squeue.size; i++)
	{
		// printf("validating: ");
		// int j = 0;
		// while(strcmp(squeue.data[i][j].value, ";"))
		// 	printf("%s", squeue.data[i][j++].value);
		// printf("%s", squeue.data[i][j].value);
		// printf("\n");

		token_t* statement_start = squeue.data[i];
		char* result = ValidateSemantics(&(squeue.data[i]), expressions, operators, &ostack);
		squeue.data[i] = statement_start;

		if(result)
			free(result);
		else
		{
			error = 1;
			break;
		}
	}

	if(expressions->size)
		PopAll(expressions);

	if(operators->size)
		PopAll(operators);

	if(!error)
	{
		for(i = predefined; i < ostack.size; i++)
		{
			free(ostack.objects[i].identifier);
			free(ostack.objects[i].type);
			free(ostack.objects[i].value);
		}

		ostack.size = predefined;

		for(i = 0; i < squeue.size; i++)
		{
	
			token_t* statement_start = squeue.data[i];
			char* result = Evaluate(&(squeue.data[i]), expressions, operators, &ostack);
			squeue.data[i] = statement_start;
	
			if(result)
				free(result);
			else
				break;
		}
	}

	if(expressions->size)
		PopAll(expressions);

	if(operators->size)
		PopAll(operators);

	for(i = 0; i < squeue.size; i++)
		free(squeue.data[i]);
	free(squeue.data);
	
	for(i = 0; i < ostack.size; i++)
	{
		free(ostack.objects[i].identifier);
		free(ostack.objects[i].type);
		free(ostack.objects[i].value);
	}
	free(ostack.objects);
}

int CreateObject(ostack_t* ostack, char* identifier, char* type, char* value, int arg_count)
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
		ResizeObjectStack(&ostack);
	}
	if(Exists(ostack, identifier) == -1)
	{
		ostack->objects[ostack->size].identifier = malloc(sizeof(char) * identifier_size);
		ostack->objects[ostack->size].type = malloc(sizeof(char) * type_size);
		ostack->objects[ostack->size].value = malloc(sizeof(char) * value_size);
		ostack->objects[ostack->size].arg_count = arg_count;

		for(i = 0; i < identifier_size; i++)
			ostack->objects[ostack->size].identifier[i] = identifier[i];

		for(i = 0; i < type_size; i++)
			ostack->objects[ostack->size].type[i] = type[i];

		for(i = 0; i < value_size; i++)
			ostack->objects[ostack->size].value[i] = value[i];

		ostack->size++;

	}
	else
	{
		printf("object %s already exists.\n", identifier);
		return 0;
	}
}

void ResizeObjectStack(ostack_t** ostack)
{
	object_t* new_objects = malloc(sizeof(object_t) * ((*ostack)->capacity + (*ostack)->capacity / 2));

	int i;
	for(i = 0; i < (*ostack)->size; i++)
	{
		new_objects[i] = (*ostack)->objects[i];
	}

	free((*ostack)->objects);
	(*ostack)->objects = new_objects;
	(*ostack)->capacity = (*ostack)->capacity + (*ostack)->capacity / 2;
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

void AddStatement(squeue_t* squeue, token_t* statement)
{
	if(squeue->size == squeue->capacity)
		Resize(&squeue);

	squeue->data[squeue->size++] = statement;
}

void Resize(squeue_t** squeue)
{
	token_t** new_data = malloc(sizeof(token_t*) * ((*squeue)->capacity + (*squeue)->capacity / 2));

	int i;
	for(i = 0; i < (*squeue)->size; i++)
	{
		new_data[i] = (*squeue)->data[i];
	}

	free((*squeue)->data);
	(*squeue)->data = new_data;
	(*squeue)->capacity = (*squeue)->capacity + (*squeue)->capacity / 2;
}

void NativeFunctionCall(const char* function_name, char** args)
{
	if(!strcmp(function_name, "write"))
	{
		// the write function
		write(args[0]);
	}
}

int length(const char* str)
{
	int i = 0;
	while(str[i] != '\0')
		i++;

	i++;
	return i;
}
