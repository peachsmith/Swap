/*
 * File: lexer.c
 * Author: John Powell
 * 
 */
#include "lexer.h"

const char WHITESPACE[3] = {' ', '\t', '\n'};
const char DIGIT[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
const char LETTER[52] = 
{
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
	'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z'
};

const char IDENTIFIER_START[54] = 
{
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
	'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
	'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', '$', '_'
};

const char SYMBOLS[41][4] = 
{
//  symbol     default meaning
	"#",	// comment start
	"\"",   // string start
	",",    // separator
	".",    // member access
	";",	// statement end
	"+",	// addition
	"-",	// subtraction
	"*",	// multiplication
	"/",	// division
	"=",	// assignment
	"!",	// not
	"<",	// less than
	">",	// greater than
	"&",	// bitwise and
	"|",	// bitwise or
	"^",    // bitwise exclusive or
	"<<",   // bit shift left
	">>",   // bit shift right
	"==",   // equal
	"!=",   // not equal
	"+=",   // addition assignment
	"-=",   // subtraction assignment
	"*=",   // multiplication assignment
	"/=",   // division assignment
	"<=",   // less than or equal
	">=",   // greater than or equal
	"&=",   // bitwise and assignment
	"|=",   // bitwise or assignment
	"^=",   // bitwise exclusive or assignment
	"<<=",  // bit shift left assignment
	">>=",  // bit shift right assignment
	"++",   // increment
	"--",   // decrement
	"&&",   // logical and
	"||",	// logical or
	"(",	// left parenthesis
	")",	// right parenthesis
	"{",	// left brace
	"}",	// right brace
	"[",	// left square bracket
	"]"		// right square bracket
};

const char KEYWORD[4][6] = 
{
	"for",
	"while",
	"if",
	"else"
};

const char PRIMITIVE[6][7] = 
{
	"int",
	"char",
	"bool",
	"long",
	"float",
	"double"
};

// token types
const char TOKEN_IDENTIFIER[11] = "identifier";
const char TOKEN_SYMBOL[7] = "symbol";
const char TOKEN_NUMBER[7] = "number";
const char TOKEN_STRING[7] = "string";

int Tokenize(jchar_t* source, token_t** tokens, int nmem)
{
	int created_tokens = 0;
	(*tokens) = malloc(sizeof(token_t) * nmem);
	jchar_t* characters = malloc(sizeof(jchar_t) * (nmem + 1));
	jchar_t token_end;

	token_end.char_data = '\0';
	characters[0] = token_end;
	int i = 0;  // jchar_t index
	int ci = 0; // character index
	int ti = 0; // token index
	int in_string = 0;
	int in_paren = 0;
	int in_square = 0;
	int in_identifier = 0;

	// tokenization loop
	while(i < nmem)
	{
		// skip whitespace
		while(IsWhitespace(source[i].char_data))
			i++;

		// skip comments
		if(source[i].char_data == '#')
		{
			while(source[i].char_data != '\n' && source[i++].char_data != '\0');
		}

		// token character collection loop
		while(source[i].char_data != '\0')
		{
			// skip whitespace, unless inside a string
			if(!in_string && IsWhitespace(source[i].char_data))
				break;

			if(source[i].char_data == '"') // entering or exiting a string
			{
				if(in_string)
				{
					if(i > 0)
					{
						if(source[i - 1].char_data != '\\')
						{
							in_string = 0;
							characters[ci++] = source[i];
							break;
						}
					}
					else
					{
						in_string = 1;
					}
				}
				else
				{
					in_string = 1;
					if(characters[0].char_data != '\0')
					{
						break;
					}
				}
			}

			else if(source[i].char_data == '+' 
				|| source[i].char_data == '-'
				|| source[i].char_data == '*'
				|| source[i].char_data == '/'
				|| source[i].char_data == '='
				|| source[i].char_data == '&'
				|| source[i].char_data == '|'
				|| source[i].char_data == '^'
				|| source[i].char_data == '<'
				|| source[i].char_data == '>'
				|| source[i].char_data == '('
				|| source[i].char_data == ')'
				|| source[i].char_data == '{'
				|| source[i].char_data == '}'
				|| source[i].char_data == '['
				|| source[i].char_data == ']'
				|| source[i].char_data == ','
				|| source[i].char_data == '.'
				|| source[i].char_data == ';')
			{
				if(!in_string)
					break;
			}

			// check for unfinished strings
			if(i < nmem)
			{
				if(source[i + 1].char_data == '\0')
				{
					if(in_string)
					{
						printf("string not properly enclosed in quotes.\n ");
						free(characters);
						return 0;
					}
				}
			}
			
			characters[ci++] = source[i++];
		} // END token character collection loop

		// make sure every token ends with token_end
		characters[ci] = token_end;

		// create token when characters[0] is token_end
		if(characters[0].char_data == '\0')
		{
			if(source[i].char_data == ';' 
				|| source[i].char_data == ','
				|| source[i].char_data == '.')
			{
				characters[0] = source[i];
				characters[1] = token_end;
				ci = 1;
				if(CreateToken(characters, &((*tokens)[ti])))
				{
					(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
					int nti;
					for(nti = 0; nti <= ci; nti++)
					{
						(*tokens)[ti].value[nti] = characters[nti].char_data;
					}
					created_tokens++;
					ti++;
				}
			}
			else if(source[i].char_data == '+'
				|| source[i].char_data == '-'
				|| source[i].char_data == '*'
				|| source[i].char_data == '/'
				|| source[i].char_data == '&'
				|| source[i].char_data == '|'
				|| source[i].char_data == '^'
				|| source[i].char_data == '<'
				|| source[i].char_data == '>'
				|| source[i].char_data == '=')
			{
				if(i < nmem)
				{
					if((source[i].char_data == '+' && source[i + 1].char_data == '+')
						|| (source[i].char_data == '-' && source[i + 1].char_data == '-')
						|| (source[i].char_data == '<' && source[i + 1].char_data == '<')
						|| (source[i].char_data == '>' && source[i + 1].char_data == '>')
						|| (source[i].char_data == '&' && source[i + 1].char_data == '&')
						|| (source[i].char_data == '|' && source[i + 1].char_data == '|'))
					{
						characters[0] = source[i];
						characters[1] = source[i + 1];
						characters[2] = token_end;
						ci = 2;

						//check for bit shift assignments <<= and >>=
						if((i < nmem - 1) && 
							(characters[1].char_data == '<' || characters[1].char_data == '>'))
						{
							if(source[i + 2].char_data == '=')
							{
								characters[2] = source[i + 2];
								characters[3] = token_end;
								ci = 3;
							}
						}

						if(CreateToken(characters, &((*tokens)[ti])))
						{
							(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
							int nti;
							for(nti = 0; nti <= ci; nti++)
							{
								(*tokens)[ti].value[nti] = characters[nti].char_data;
							}
							created_tokens++;
							ti++;
						}
						characters[0] = token_end;
						i += ci;
						ci = 0;
						continue;
					}
					else if(source[i + 1].char_data == '=')
					{
						characters[0] = source[i];
						characters[1] = source[i + 1];
						characters[2] = token_end;
						ci = 2;
						if(CreateToken(characters, &((*tokens)[ti])))
						{
							(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
							int nti;
							for(nti = 0; nti <= ci; nti++)
							{
								(*tokens)[ti].value[nti] = characters[nti].char_data;
							}
							created_tokens++;
							ti++;
						}
						ci = 0;
						characters[ci] = token_end;
						i += 2;
						continue;
					}
					else
					{
						characters[0] = source[i];
						characters[1] = token_end;
						ci = 1;
						if(CreateToken(characters, &((*tokens)[ti])))
						{
							(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
							int nti;
							for(nti = 0; nti <= ci; nti++)
							{
								(*tokens)[ti].value[nti] = characters[nti].char_data;
							}
							created_tokens++;
							ti++;
						}
					}
				}
				else
				{
					characters[0] = source[i];
					characters[1] = token_end;
					ci = 1;
					if(CreateToken(characters, &((*tokens)[ti])))
					{
						(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
						int nti;
						for(nti = 0; nti <= ci; nti++)
						{
							(*tokens)[ti].value[nti] = characters[nti].char_data;
						}
						created_tokens++;
						ti++;
					}
				}
			}
			else if(source[i].char_data == '('
				|| source[i].char_data == ')'
				|| source[i].char_data == '{'
				|| source[i].char_data == '}'
				|| source[i].char_data == '['
				|| source[i].char_data == ']')
			{
				characters[0] = source[i];
				characters[1] = token_end;
				ci = 1;
				if(CreateToken(characters, &((*tokens)[ti])))
				{
					(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
					int nti;
					for(nti = 0; nti <= ci; nti++)
					{
						(*tokens)[ti].value[nti] = characters[nti].char_data;
					}
					created_tokens++;
					ti++;
				}
			}
			i++;
			ci = 0;
			characters[0] = token_end;
			continue;
		} // END create token when characters[0] is token_end

		else if(source[i].char_data == '"')
		{
			if(characters[0].char_data != '"')
			{
				if(CreateToken(characters, &((*tokens)[ti])))
				{
					(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
					int nti;
					for(nti = 0; nti <= ci; nti++)
					{
						(*tokens)[ti].value[nti] = characters[nti].char_data;
					}
					created_tokens++;
					ti++;
				}
				characters[0] = source[i];
				ci = 1;
				i++;
				continue;
			}
			if(CreateToken(characters, &((*tokens)[ti])))
			{
				(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
				int nti;
				for(nti = 0; nti <= ci; nti++)
				{
					(*tokens)[ti].value[nti] = characters[nti].char_data;
				}
				created_tokens++;
				ti++;
			}
			ci = 0;
			characters[ci] = token_end;
			i++;
			continue;
		}

		if(CreateToken(characters, &((*tokens)[ti])))
		{
			(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
			int nti;
			for(nti = 0; nti <= ci; nti++)
			{
				(*tokens)[ti].value[nti] = characters[nti].char_data;
			}
			created_tokens++;
			ti++;
		}
		else
		{
			const problem_size = ci;
			char problem_token[problem_size];
			int nti;
			for(nti = 0; nti <= problem_size; nti++)
			{
				problem_token[nti] = characters[nti].char_data;
			}
			printf("invalid token: %s at row %i, column %i\n", problem_token, 
				characters[0].row, characters[0].column);
			free(characters);
			return 0;
		}

		// handle symbols right after identifiers
		if(source[i].char_data == ';' 
			|| source[i].char_data == ','
			|| source[i].char_data == '.')
		{
			characters[0] = source[i];
			characters[1] = token_end;
			ci = 1;
			if(CreateToken(characters, &((*tokens)[ti])))
			{
				(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
				int nti;
				for(nti = 0; nti <= ci; nti++)
				{
					(*tokens)[ti].value[nti] = characters[nti].char_data;
				}
				created_tokens++;
				ti++;
			}
		}
		else if(source[i].char_data == '+'
			|| source[i].char_data == '-'
			|| source[i].char_data == '*'
			|| source[i].char_data == '/'
			|| source[i].char_data == '&'
			|| source[i].char_data == '|'
			|| source[i].char_data == '^'
			|| source[i].char_data == '>'
			|| source[i].char_data == '<'
			|| source[i].char_data == '=')
		{
			if(i < nmem)
			{
				if((source[i].char_data == '+' && source[i + 1].char_data == '+')
					|| (source[i].char_data == '-' && source[i + 1].char_data == '-')
					|| (source[i].char_data == '<' && source[i + 1].char_data == '<')
					|| (source[i].char_data == '>' && source[i + 1].char_data == '>')
					|| (source[i].char_data == '&' && source[i + 1].char_data == '&')
					|| (source[i].char_data == '|' && source[i + 1].char_data == '|'))
				{
					characters[0] = source[i];
					characters[1] = source[i + 1];
					characters[2] = token_end;
					ci = 2;
					//check for bit shift assignments <<= and >>=
					if((i < nmem - 1) && 
						(characters[1].char_data == '<' || characters[1].char_data == '>'))
					{
						if(source[i + 2].char_data == '=')
						{
							characters[2] = source[i + 2];
							characters[3] = token_end;
							ci = 3;
						}
					}

					if(CreateToken(characters, &((*tokens)[ti])))
					{
						(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
						int nti;
						for(nti = 0; nti <= ci; nti++)
						{
							(*tokens)[ti].value[nti] = characters[nti].char_data;
						}
						created_tokens++;
						ti++;
					}
					
					characters[0] = token_end;
					i += ci;
					ci = 0;
					continue;
				}
				else if(source[i + 1].char_data == '=')
				{
					characters[0] = source[i];
					characters[1] = source[i + 1];
					characters[2] = token_end;
					ci = 2;
					if(CreateToken(characters, &((*tokens)[ti])))
					{
						(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
						int nti;
						for(nti = 0; nti <= ci; nti++)
						{
							(*tokens)[ti].value[nti] = characters[nti].char_data;
						}
						created_tokens++;
						ti++;
					}
					ci = 0;
					characters[ci] = token_end;
					i += 2;
					continue;
				}
				else
				{
					characters[0] = source[i];
					characters[1] = token_end;
					ci = 1;
					if(CreateToken(characters, &((*tokens)[ti])))
					{
						(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
						int nti;
						for(nti = 0; nti <= ci; nti++)
						{
							(*tokens)[ti].value[nti] = characters[nti].char_data;
						}
						created_tokens++;
						ti++;
					}
				}
			}
			else
			{
				characters[0] = source[i];
				characters[1] = token_end;
				ci = 1;
				if(CreateToken(characters, &((*tokens)[ti])))
				{
					(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
					int nti;
					for(nti = 0; nti <= ci; nti++)
					{
						(*tokens)[ti].value[nti] = characters[nti].char_data;
					}
					created_tokens++;
					ti++;
				}
			}
		}
		else if(source[i].char_data == '('
			|| source[i].char_data == ')'
			|| source[i].char_data == '{'
			|| source[i].char_data == '}'
			|| source[i].char_data == '['
			|| source[i].char_data == ']')
		{
			characters[0] = source[i];
			characters[1] = token_end;
			ci = 1;
			if(CreateToken(characters, &((*tokens)[ti])))
			{
				(*tokens)[ti].value = malloc(sizeof(char) * (ci + 1));
				int nti;
				for(nti = 0; nti <= ci; nti++)
				{
					(*tokens)[ti].value[nti] = characters[nti].char_data;
				}
				created_tokens++;
				ti++;
			}
		}
		i++;
		ci = 0;
		characters[0] = token_end;
	} // END tokenization loop

	free(characters);

	if(in_string)
	{
		printf("string not properly enclosed in quotes.\n");
		return 0;
	}

	return created_tokens;
}

int CreateToken(jchar_t* characters, token_t* token)
{
	int token_size = 0;
	int i;
	while(characters[token_size++].char_data != '\0');

	const int char_size = token_size;

	char raw_chars[char_size];

	for(i = 0; i < token_size; i++)
	{
		raw_chars[i] = characters[i].char_data;
	}

	if(IsNumber(raw_chars))
		(*token).type = "number";
	else if(IsString(raw_chars))
		(*token).type = "string";
	else if(IsKeyword(raw_chars))
		(*token).type = "keyword";
	else if(IsIdentifier(raw_chars))
		(*token).type = "identifier";
	else if(IsSymbol(raw_chars))
		(*token).type = "symbol";
	else
	{
		(*token).type = "invalid";
		return 0;
	}

	(*token).row = characters[0].row;
	(*token).column = characters[0].column;

	return 1;
}

int IsWhitespace(char character)
{
	int i;
	for(i = 0; i < 3; i++)
	{
		if(character == WHITESPACE[i])
			return 1;
	}
	return 0;
}

int IsDigit(char character)
{
	int i;
	for(i = 0; i < 10; i++)
	{
		if(character == DIGIT[i])
			return 1;
	}
	return 0;
}

int IsLetter(char character)
{
	int i;
	for(i = 0; i < 52; i++)
	{
		if(character == LETTER[i])
			return 1;
	}
	return 0;
}

int IsNumber(char* characters)
{
	if(!IsDigit(characters[0]) || characters[0] == '0')
		return 0;

	int i = 1;
	while(characters[i] != '\0')
	{
		if(!IsDigit(characters[i++]))
			return 0;
	}
	return 1;
}

int IsString(char* characters)
{
	if(characters[0] != '"')
		return 0;

	if(characters[1] == '\0')
		return 0;

	int i = 1;
	while(characters[i] != '\0')
	{
		if((characters[i] == '\0'))
		{
			if(characters[i - 1] != '"')
				return 0;
		}
		i++;
	}

	return 1;
}

int IsKeyword(char* characters)
{
	int i;
	for(i = 0; i < 4; i++)
	{
		if(!strcmp(characters, KEYWORD[i]))
			return 1;
	}
	return 0;
}

int IsIdentifier(char* characters)
{
	if(!IsLetter(characters[0]) && characters[0] != '$' && characters[0] != '_')
		return 0;

	if(characters[1] == '\0')
		return 1;

	int i = 1;
	while(characters[i] != '\0')
	{
		if(!IsLetter(characters[i]) && IsDigit(characters[i])
		&& characters[i] == '$' && characters[i] == '_')
		{
			return 0;
		}
		i++;
	}

	return 1;
}

int IsSymbol(char* characters)
{
	int i;
	for(i = 0; i < 41; i++)
	{
		if(!strcmp(characters, SYMBOLS[i]))
			return 1;
	}
	return 0;
}

// not currently used
int IsPrimitive(char* characters)
{
	int i;
	for(i = 0; i < 6; i++)
	{
		if(!strcmp(characters, PRIMITIVE[i]))
			return 1;
	}
	return 0;
}
