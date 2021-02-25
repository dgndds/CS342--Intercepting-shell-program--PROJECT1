#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int numOfChars = atoi(argv[1]);
	
	const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	
	srand((unsigned)time(NULL) * getpid());
	
	for(int i = 0; i < numOfChars; i++){
		printf("%c\n",alphanum[rand() % (sizeof(alphanum) - 1)]);
	}
	return 0;
}
