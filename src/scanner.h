/*
 * File: scanner.h
 * Author: John Powell
 *
 * Due to the way in which the program calculates file size, files
 * with a size of 2GB or larger.
 */
#ifndef JEP_SCANNER_H_
#define JEP_SCANNER_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct Character
{
	char char_data;
	int row;
	int column;
} jchar_t;

int ScanSource(char* file_name, jchar_t** source);
void PrintScanResult(jchar_t* source);

#endif
