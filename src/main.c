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

int main(int argc,char** argv)
{
	args_t args;
	jchar_t* source;
	token_t* tokens;
	tstream_t token_stream;
	stack_t opr_stack; // operator stack
	stack_t exp_stack; // expression stack
	char* file_name;
	int character_count;
	int token_count = 0;

	args.input = 0;
	args.output = 0;
	args.flag_v = 0;

	CollectArguments(argc, argv, &args);

	if(argc == 1)
	{
		printf("SWAP (solution without a problem)\nAuthor: John Powell\n");
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
				opr_stack.capacity = 10;
				opr_stack.data = malloc(sizeof(char*) * opr_stack.capacity);
				opr_stack.size = 0;

				// initialize expression stack
				exp_stack.capacity = 10;
				exp_stack.data = malloc(sizeof(char*) * exp_stack.capacity);
				exp_stack.size = 0;

				// point the token stream at the first token
				token_stream.next = &tokens[0];

				Interpret(token_stream.next, &exp_stack, &opr_stack);

				//free the memory from the stacks
				free(opr_stack.data);
				free(exp_stack.data);
			}

		}

	}

	if(args.output)
	{
		// TODO have some sort of output
	}

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
