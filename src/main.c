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
int indent;
int curlies;

void NextToken();
int Accept(const char* expected);
int Expect(const char* expected);
void Factor();
void Term();
void Expression();
void Statement();
void Block();

int main(int argc,char** argv)
{
	char version[15] = "1.0.0";
	args_t args;
	jchar_t* source;
	token_t* tokens;
	char* file_name;
	int character_count;
	int token_count = 0;

	indent = 0;
	curlies = 0;

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
			next = &tokens[0];

			do
			{
				Block();
			}
			while(strcmp((*next).value, "[end]"));

			if(curlies)
				printf("mismatched curly braces\n");
			else
				printf("\n");

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
	//printf("expected %s found %s\n",expected, (*next).value );
	if(!strcmp(expected, "number") 
		|| !strcmp(expected, "identifier")
		|| !strcmp(expected, "keyword"))
	{
		if(!strcmp((*next).type, expected))
		{
			printf("%s", (*next).value);
			NextToken();
			return 1;
		}
		else
			return 0;
	}

	if(!strcmp((*next).value, expected))
	{
		if(!strcmp((*next).value, "{") || !strcmp((*next).value, "}"))
		{
			if(indent)
				printf("\n%*s",indent-1,(*next).value );
			else
				printf("\n%*s",indent,(*next).value );
		}
		else
			printf("%s", (*next).value);
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
		printf("\nunexpected token. expected: %s  found %s\n", expected, (*next).value);
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
		printf("\nsyntax error\n");
		NextToken();
	}
}

void Term()
{
	Factor();
	while (Accept("*") || Accept("/"))
		Factor();
}

void Expression()
{
	if(Accept("+") || Accept("-"));
	Term();
	while(Accept("+") || Accept("-"))
		Term();
}

void Statement()
{
	printf("\n%*s", indent, "");
	indent += 2;
	if(Accept(";"))
	{
		// empty statement
	}
	else if(Accept("if"))
	{
		Expect("(");

		Expression();

		Expect(")");

		Block();
	}
	else
	{
		Expression();
		Expect(";");
	}
	indent -= 2;
}

void Block()
{
	if(Accept("{"))
	{
		curlies++;
		do
		{
			Statement();
		}
		while(strcmp((*next).value, "}") && strcmp((*next).value, "[end]"));
		Expect("}");
		curlies--;
	}
	else
	{
		Statement();
		if(Accept("}"))
		{
			printf("\nerror: unexpected '}'\n");
			NextToken();
		}
	}
}
