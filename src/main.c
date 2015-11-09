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

void NextToken();
int Accept(const char* expected);
int Expect(const char* expected);
void Factor();
void Term();
void Expression();
void Condition();
void Statement();

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

			Statement();

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

void NextToken()
{
	(*next++);
}

int Accept(const char* expected)
{
	printf("expected %s found %s\n",expected, (*next).value );
	if(!strcmp(expected, "number") || !strcmp(expected, "identifier"))
	{
		if(!strcmp((*next).type, expected))
		{
			//printf("%s", (*next).value);
			NextToken();
			return 1;
		}
		else
			return 0;
	}

	if(!strcmp((*next).value, expected))
	{
		//printf("%s", (*next).value);
		NextToken();
		return 1;
	}
	else 
		return 0;
}

int Expect(const char* expected)
{
	if(Accept(expected))
		return 1;
	else
		printf("unexpected token. expected: %s  found %s\n", expected, (*next).value);
	return 0;
}

void Factor()
{
	if(Accept("identifier"))
	{

	}
	else if(Accept("number"))
	{

	}
	else if(Accept("("))
	{
		Expression();
		Expect(")");
	}
	else
	{
		printf("syntax error.\n");
		NextToken();
	}
}

void Term()
{
	Accept("*");
	Accept("/");
	Factor();
	while (Accept("*") || Accept("/"))
		Factor();
}

void Expression()
{
	Accept("+");
	Accept("-");
	Term();
	while(Accept("+") || Accept("-"))
		Term();
}

void Condition()
{
	Expression();
	if(Accept("==") || Accept("!=") || Accept("<") || Accept(">")
		|| Accept("<=") || Accept(">="))
	{
		Expression();
	}
	else
	{
		printf("Condition: invalid operator\n");
		NextToken();
	}
}

void Statement()
{
	if(Accept("("))
	{
		Expression();
		Expect(")");
		if(strcmp((*next).value,";"))
			Expression();
	}
	if(Accept("identifier") || Accept("number"))
	{
		if(Accept("=") || Accept("+=") || Accept("-=") || Accept("*=")
			|| Accept("/=") || Accept("<<=") || Accept(">>=")) /* assignment */
		{
			Expression();
		}
		else if(Accept(";"))
		{
			printf("found a declaration\n");
			return;
		}
		else
		{
			Expression();
		}
	}
	if(Expect(";"))
	{
		printf("\nfound a valid statement.\n");
	}
	else
		printf("\ncould not find a valid statement.\n");
}
