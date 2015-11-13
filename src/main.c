#include <stdio.h>
#include <string.h>

#include "parser.h"

typedef struct Arguments
{
	char* input;
	char* output;
	int flag_v;
} args_t;

int CollectArguments(int argc, char** argv, args_t* args);

typedef struct Stack
{
	char** data;
	int capacity;
	int size;
} stack_t;

void Push(stack_t* stack, char* value);
void Pop(stack_t* stack, char** value);
void PopAll(stack_t* stack);
int Priority(char* token);
void PrintStack(stack_t* stack);
void EvaluateBinaryOperation(char** operator, char** l_operand, char** r_operand, char** result);

int main(int argc,char** argv)
{
	char version[15] = "1.0.0";
	args_t args;
	jchar_t* source;
	token_t* tokens;
	tstream_t token_stream;
	stack_t o_stack; // operator stack
	stack_t e_stack; // expression stack
	char* file_name;
	int character_count;
	int token_count = 0;

	args.input = 0;
	args.output = 0;
	args.flag_v = 0;

	CollectArguments(argc, argv, &args);

	if(argc == 1)
	{
		printf("SWAP (solution without a problem)\nAuthor: John Powell\nversion: %s", version);
		return 0;
	}

	if(args.input)
	{
		character_count = ScanSource(args.input, &source);

		if(character_count == -1)
		{
			printf("could not open file\n");
			return -1;
		}
		
		token_count = Tokenize(source, &tokens, character_count);
		
		if(token_count > 1)
		{
			/* get the first token */
			token_stream.next = &tokens[0];
			token_stream.indent = 0;
			token_stream.syntax_error = 0;

			// initialize AST
			(*token_stream.ast_node).value = "root";
			(*token_stream.ast_node).type = ROOT;

			do
			{
				Block(&token_stream);
			}
			while(strcmp(token_stream.next->value, "end of stream"));

			printf("\n");

			// expression evaluating nonsense
			if(!token_stream.syntax_error)
			{
				// initialize operator stack
				o_stack.capacity = 10;
				o_stack.data = malloc(sizeof(char*) * o_stack.capacity);
				o_stack.size = 0;

				// initialize expression stack
				e_stack.capacity = 10;
				e_stack.data = malloc(sizeof(char*) * e_stack.capacity);
				e_stack.size = 0;

				// point the token stream at the first token
				token_stream.next = &tokens[0];

				while(strcmp(token_stream.next->value, "end of stream"))
				{
					if(!strcmp(token_stream.next->type, "number") 
						|| !strcmp(token_stream.next->type, "identifier"))
					{
						Push(&e_stack, token_stream.next->value);
					}
					else if(!strcmp(token_stream.next->type, "symbol")
						&& strcmp(token_stream.next->value, "{") 
						&& strcmp(token_stream.next->value, "}")
						&& strcmp(token_stream.next->value, ";"))
					{
						if(o_stack.size)
						{
							//printf("next token: %s\n",token_stream.next->value);
							if(Priority(token_stream.next->value) < Priority(o_stack.data[o_stack.size - 1]))
							{
								char* l_op;
								char* r_op;
								char* opr;
								char* result; // malloc
								Pop(&e_stack, &r_op);
								Pop(&e_stack, &l_op);
								Pop(&o_stack, &opr);

								EvaluateBinaryOperation(&opr, &l_op, &r_op, &result);
								Push(&e_stack, result);
								Push(&o_stack, token_stream.next->value);

								free(e_stack.data[e_stack.size + 1]);
								free(e_stack.data[e_stack.size]);
								free(o_stack.data[o_stack.size]);
								free(result);
							}
							else
								Push(&o_stack, token_stream.next->value);
						}
						else
							Push(&o_stack, token_stream.next->value);
					}
					else if(!strcmp(token_stream.next->value, ";"))
					{
						printf("------------\noperator stack\n");
						PrintStack(&o_stack);
						PopAll(&o_stack);
						printf("expression stack\n");
						PrintStack(&e_stack);
						PopAll(&e_stack);
					}
					token_stream.next++;
				}

				//free the memory from the stacks
				free(o_stack.data);
				free(e_stack.data);
			}

			if(args.flag_v)
			{
				int ti;
				token_t* token = &tokens[0];
				for(ti = 0; ti < token_count; ti++)
				{
					printf("%-*s  %s\n", 12, token->type, token->value);
					*token++;
				}
			}
		}

	}
	/*
	if(args.flag_v)
	{
		if(args.input)
			PrintScanResult(source);
		else
			printf("no input file\n");
	}
	*/

	if(args.output)
		PrintScanResultToFile(character_count, source, args.output);

	free(source);

	int token_index;
	for(token_index = 0; token_index < token_count - 1; token_index++)
	{
		free(tokens[token_index].value);
	}

	free(tokens);

	return 0;
}

int CollectArguments(int argc, char** argv, args_t* args)
{
	if(argc < 1)
		return 0;

	int flag_o = 0;

	int i;
	for(i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-o"))
		{
			if(argc < i + 2)
				return -1;
			if(flag_o)
			{
				printf("too many output flags\n");
				return -1;
			}
			flag_o = 1;
			continue;
		}
		else if(!strcmp(argv[i], "-v"))
		{
			if((*args).flag_v)
			{
				printf("too many verbose flags\n");
					return -1;
			}
			(*args).flag_v = 1;
			continue;
		}

		if(!strcmp(argv[i - 1], "-o"))
		{
			(*args).output = argv[i];
			flag_o = 1;
		}
		else if(strcmp(argv[i - 1], "-o") && strcmp(argv[i], "-o") 
			&& strcmp(argv[i], "-v"))
		{
			(*args).input = argv[i];
		}
	}
	return 1;
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
		printf("%s", stack->data[i]);
	printf("\n");
}

int Priority(char* value)
{
	if(!strcmp(value, "+") || !strcmp(value, "-"))
		return 1;
	else if(!strcmp(value, "*") || !strcmp(value, "/"))
		return 2;
	else return 0;
}

void EvaluateBinaryOperation(char** operator,  char** l_operand, char** r_operand, char** result)
{
	char l_buffer[30];
	char r_buffer[30];
	char result_buffer[30];
	int l_op;
	int r_op; 
	sscanf(*l_operand, "%d", &l_op);
	sscanf(*r_operand, "%d", &r_op);
	int i = 0;

	if(!strcmp(*operator, "+"))
	{
		sprintf(result_buffer, "%d", l_op + r_op);
		*result = malloc(sizeof(char) * 30);
		while(result_buffer[i] != '\0')
			(*result)[i] = result_buffer[i++];
		(*result)[i] = result_buffer[i];
	}
	else if(!strcmp(*operator, "-"))
	{
		sprintf(result_buffer, "%d", l_op - r_op);
		*result = malloc(sizeof(char) * 30);
		while(result_buffer[i] != '\0')
			(*result)[i] = result_buffer[i++];
		(*result)[i] = result_buffer[i];
	}
	else if(!strcmp(*operator, "*"))
	{
		sprintf(result_buffer, "%d", l_op * r_op);
		*result = malloc(sizeof(char) * 30);
		while(result_buffer[i] != '\0')
			(*result)[i] = result_buffer[i++];
		(*result)[i] = result_buffer[i];
	}
	else if(!strcmp(*operator, "/"))
	{
		sprintf(result_buffer, "%d", l_op / r_op);
		*result = malloc(sizeof(char) * 30);
		while(result_buffer[i] != '\0')
			(*result)[i] = result_buffer[i++];
		(*result)[i] = result_buffer[i];
	}

}
