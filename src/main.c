#include <stdio.h>
#include <string.h>

#include "lexer.h"

// this is a struct since more args may be accepted in the future
typedef struct Arguments
{
	char* input;
	char* output;
	int flag_v;
} args_t;

int CollectArguments(int argc, char** argv, args_t* args);

token_t* next;

int Expect(const char* expected)
{
	if(!strcmp(expected, "number") || !strcmp(expected, "identifier"))
	{
		if(!strcmp((*next++).type, expected))
			return 1;
		else
			return 0;
	}

	if(!strcmp((*next++).value, expected))
		return 1;
	else 
		return 0;
}

int E1() { return Term(); }
int E2() { return Term() && Expect("+") && Expression(); }

int Expression() 
{ 
	token_t *save = next; 
	return (next = save, E1()) || (next = save, E2()); 
}

int T1() { return Expect("number"); }
int T2() { return Expect("number") && Expect("*") && Term(); }
int T3() { return Expect("(") && Expression() && Expect(")"); }

int Term() 
{
	token_t* save = next;
	return (next = save, T1()) 
	|| (next = save, T2())
	|| (next = save, T3());
}

int main(int argc,char** argv)
{
	char version[15] = "1.0.0";
	args_t args;
	jchar_t* source;
	token_t* tokens;
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
		
		if(token_count)
		{
			/* get the first token */
			next = &tokens[0];

			if(Expression())
			{
				printf("found a valid expression!\n");
			}
			else
			{
				printf("did not find any valid expressions.\n");
			}

			if(args.flag_v)
			{
				int ti;
				token_t* token = &tokens[0];
				for(ti = 0; ti < token_count; ti++)
				{
					printf("%-*s  %s\n", 12, (*token).type, (*token).value);
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
	for(token_index = 0; token_index < token_count; token_index++)
		free(tokens[token_index].value);

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
