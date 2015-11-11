#include "parser.h"

void NextToken(tstream_t* stream)
{
	(*stream).next++;
}

int Accept(const char* expected, tstream_t* stream)
{
	//printf("expected %s found %s\n",expected, (*(*stream).next).value);
	if(!strcmp(expected, "number") 
		|| !strcmp(expected, "identifier")
		|| !strcmp(expected, "keyword"))
	{
		if(!strcmp((*(*stream).next).type, expected))
		{
			printf("%s", (*(*stream).next).value);
			NextToken(stream);
			return 1;
		}
		else
			return 0;
	}

	if(!strcmp((*(*stream).next).value, expected))
	{
		
		if(!strcmp((*(*stream).next).value, "{") || !strcmp((*(*stream).next).value, "}"))
		{
			if((*stream).indent)
				printf("\n%*s",(*stream).indent-1,(*(*stream).next).value);
			else
				printf("\n%*s",(*stream).indent,(*(*stream).next).value);
		}
		else
			printf("%s", (*(*stream).next).value);
			
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
		printf("\nunexpected token row: %d column: %d. expected: %s  found %s\n", (*(*stream).next).row, (*(*stream).next).column, expected, (*(*stream).next).value);
		(*stream).syntax_error = 2;
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
		printf("\nsyntax error\n");
		(*stream).syntax_error = 1;
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
	printf("\n%*s", (*stream).indent, "");
	(*stream).indent += 2;
	if(Accept(";",stream))
	{
		// empty statement
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
	(*stream).indent -= 2;
}

void Block(tstream_t* stream)
{
	if(Accept("{",stream))
	{
		if(!Accept("}",stream))
		{
			while(strcmp((*(*stream).next).value, "}") && strcmp((*(*stream).next).value, "end of stream"))
				Statement(stream);
			Expect("}",stream);

		}
	}
	else
	{
		Statement(stream);
	}
}