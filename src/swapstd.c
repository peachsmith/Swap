#include "swapstd.h"

void write(const char* output)
{
	int i;
	int output_size = 0;

	while(output[output_size] != '\0')
		output_size++;

	output_size++;

	if(output[0] == '"')
	{
		// remove outer quotes
		char *unquoted = malloc(sizeof(char) * (output_size - 2));

		for(i = 1; i < output_size - 2; i++)
			unquoted[i - 1] = output[i];

		unquoted[i - 1] = '\0';

		printf("%s\n", unquoted);

		free(unquoted);
	}
	else
		printf("%s\n", output);
}