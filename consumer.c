#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int numOfChar = atoi(argv[1]);
	char chr[numOfChar];
	
	for(int i = 0; i < numOfChar; i++){
		 scanf(" %c",&chr[i]);
	}
	
	printf("CHARS READ:\n");
	
	for(int i = 0; i < numOfChar; i++){
		printf("%c\n",chr[i]);
	}
}

