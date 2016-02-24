/*
 * File: scanner.c
 * Author: John Powell
 *
 * Due to the way in which the program calculates file size, files
 * with a size of 2GB or larger.
 */
#include <stdio.h>
#include "scanner.h"

int ScanSource(char* file_name, jchar_t** source)
{
	FILE* fp = 0;
	long int file_size = 0;
	size_t source_length = 0;
	int row = 1;
	int column = 1;
	int i;
	char* buffer;

	fp = fopen(file_name,"r");
	fseek(fp, 0L, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	
	if(!fp)
	{
		return -1;
	}
	else
	{
		buffer = malloc(file_size + 1);
		source_length = fread(buffer, sizeof(char), file_size, fp);
		buffer[source_length] = '\0';
		*source = malloc((source_length + 1) * sizeof(jchar_t));

		for(i = 0; i <= source_length; i++)
		{
			(*source)[i].char_data = buffer[i];
			(*source)[i].column = column++;
			if(buffer[i] == '\n')
			{
				(*source)[i].row = row++;
				column = 1;
			}
			else
				(*source)[i].row = row;
		}
	}

	fclose(fp);
	free(buffer);

	return source_length;
}

void PrintScanResult(jchar_t* source)
{
	int i = 0; //i = 0; i < buffer_size; i++
	for(;;)
	{
		if(source[i].char_data == '\n')
			printf("%-*s ", 12, "[newline]");
		else if(source[i].char_data == '\t')
			printf("%-*s ", 12, "[tab]");
		else if(source[i].char_data == ' ')
			printf("%-*s ", 12, "[space]");
		else if(source[i].char_data == '\0')
			printf("%-*s ", 12, "[null]");
		else
			printf("%-*c ", 12, source[i].char_data);

		printf("%i:%i\n", source[i].row, source[i].column);

		if(source[i].char_data == '\0')
			break;
		i++;
	}
}
