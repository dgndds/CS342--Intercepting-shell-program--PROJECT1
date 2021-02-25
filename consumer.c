#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	int numOfChar = atoi(argv[1]);
	char chr[numOfChar];
	int counter = 0;
	
	for(int i = 0; i < numOfChar; i++){
		 scanf(" %c\n",&chr[i]);
		 counter++;
	}
	
	printf("CHARS READ: %d\n",counter);
	printf("argv: %d\n",numOfChar);
	
	for(int i = 0; i < numOfChar; i++){
		printf("%c\n",chr[i]);
	}
	
	return 0;
}

