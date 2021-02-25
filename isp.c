#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

char** getParams(char* str1){
	char *str = malloc(strlen(str1) + 1);
	strcpy(str,str1);
	char *param = strtok(str," ");
	char** result = NULL;
	int space = 0;
	
	while(param){
		result = realloc(result , sizeof(char*)*++space);
		
		//if(result == NULL)
			//return;
		
		result[space - 1] = param;
		
		param = strtok(NULL," ");
	}
	
	result = realloc(result,sizeof(char*)*(space + 1));
	result[space] = 0;
	
	return result;
}

int main(int argc, char *argv[]){
	
	char *input = malloc(150);
	char *input1 = malloc(150);
	char *input2 = malloc(150);
	char *ignore = malloc(150);
	char** params1;
	char** params2;
	
	if(argc != 3){
		printf("INAPPROPRIATE ARGUMENT COUNT \n");
		return -1;
	}
	
	int bufferSize = atoi(argv[1]);
	int mode = atoi(argv[2]);
	
	if(bufferSize < 0 || bufferSize > 4096){
		printf("------ INAPPROPRIATE BUFFER SIZE VALUE ------\n");
		return -1;
	}
	
	if(mode <= 0 || mode > 2){
		printf("------ INAPPROPRIATE MODE VALUE ------\n");
		return -1;
	}
	
	if(mode == 1){
		printf("--------- NORMAL MODE ---------\n");
	}else if(mode == 2){
		printf("--------- TAPPED MODE ---------\n");
	}
	
	while(1){
		//INIT AND CLEAR BUFFERS
		input = strdup("");
		input1 = strdup("");
		input2 = strdup("");
		
		//printf("argv 0: %s\n", argv[0]);
		//printf("argv 1: %s\n", argv[1]);
		//printf("argv 2: %s\n", argv[2]);
	
		// READ USER INPUT
		printf("\nisp$ ");
		fgets(input,150,stdin);
		sscanf(input,"%[^\n]",input);
		
		if(strcmp(input,"\n") == 0){
			printf("DUDE NO PLEASE!!!\n");
			continue;
		}
		
		//SCAN THE INPUT AND CHECK FORMAT
		int result = sscanf(input, "%[^|^\t^\n]| %[^|^\t^\n]| %[^|^\t^\n]", input1, input2,ignore);
		
		
		if(result > 2){
			printf("DUDE NO PLEASE!!!\n");
			continue;
		}else if(result == 2){
			if(strcmp(input2,"")==0){
				printf("DUDE NO PLEASE!!!\n");
				continue;
			}
		}else if(result == 1){
			if(strcmp(input1,"")==0){
				printf("DUDE NO PLEASE!!!\n");
				continue;
			}
		}else if(result == 0){
			printf("DUDE NO PLEASE!!!\n");
			continue;
		}
		
		//ANALYZE COMMANDS
		
		if(strcmp(input1,"") != 0){
			params1 = getParams(input1);
		}
		
		if(strcmp(input2,"") != 0){
			params2 = getParams(input2);
		}
		
		struct timeval timeval1;
		struct timeval timeval2;
		
		gettimeofday(&timeval1,NULL);
		
		//EXECUTE COMMANDS
		if(strcmp(input1,"")!= 0 && strcmp(input2,"")==0){
			//SINGLE COMMAND EXECUTION
			
			pid_t pid = fork();
			
			if(pid < 0){
				printf("Execution of the command failed!");
			}else if(pid == 0){
				//Child process
				//printf("Executing %s\n",input1);
				execvp(params1[0],params1);
				break;
			}else{
				wait(NULL);
				//printf("Child completed!\n");
			}
		}else if(strcmp(input1,"")!= 0 && strcmp(input2,"")!=0){
			//COMPOUND COMMAND EXECUTION
			
			if(mode == 1){
				// NORMAL MODE EXECUTION
				int fd[2];
				
				if(pipe(fd) == -1){
					printf("PIPE FAILED\n");
					return -1;
				}
				
				int pid1 = fork();
				
				if(pid1 < 0){
					//Fork failed
					printf("FORK FAILED");
					return 3;
				}else if(pid1 == 0){
					//Child process 1
					//exec...
					dup2(fd[1],STDOUT_FILENO);
					close(fd[0]);
					close(fd[1]);
					execvp(params1[0],params1);
				}
				
				int pid2 = fork();
				
				if(pid2 < 0){
					//Fork failed
					printf("FORK FAILED");
					return 3;
				}else if(pid2 == 0){
					//Child process 2
					//exec...
					dup2(fd[0],STDIN_FILENO);
					close(fd[0]);
					close(fd[1]);
					execvp(params2[0],params2);
				}
				
				//Wait Childs
				close(fd[0]);
				close(fd[1]);
				waitpid(pid1,NULL,0);
				waitpid(pid2, NULL,0);
				//printf("Childs completed!\n");
			}else if(mode == 2){
				//TAPED MODE EXECUTION
				int fd1[2];
				int fd2[2];
				
				if(pipe(fd1) == -1 || pipe(fd2) == -1){
					printf("PIPE FAILED\n");
					return -1;
				}
				
				int pid1 = fork();
				
				if(pid1 < 0){
					//Fork failed
					printf("FORK FAILED");
					return 3;
				}else if(pid1 == 0){
					//Child process 1
					dup2(fd1[1],STDOUT_FILENO);
					//write(fd[1],);
					close(fd1[0]);
					close(fd1[1]);
					close(fd2[0]);
					close(fd2[1]);	
					execvp(params1[0],params1);
				}
				
				int pid2 = fork();
				
				if(pid2 < 0){
					//Fork failed
					printf("FORK FAILED");
					return 3;
				}else if(pid2 == 0){
					//Child process 2
					dup2(fd2[0],STDIN_FILENO);
					close(fd1[0]);
					close(fd1[1]);
					close(fd2[0]);
					close(fd2[1]);	
					execvp(params2[0],params2);
				}				
				
				
				close(fd1[1]);
				close(fd2[0]);
				
				char buffer[bufferSize];
				int Rcounter = 0;
				int Wcounter = 0;
				int charCount = 0;
				int charCountSum = 0;
		
				charCount = read(fd1[0],buffer,bufferSize);
				
				while(charCount != 0){
					write(fd2[1],buffer,bufferSize);
					Wcounter++;
					//printf("Buffer length: %lu\n",strlen(buffer));
					//printf("writing to input:%d\n",counter);
					charCountSum += charCount;
					charCount = read(fd1[0],buffer,bufferSize);
					Rcounter++;
				}
				
				close(fd1[0]);
				close(fd2[1]);	
				
				//Wait Childs
				waitpid(pid1,NULL,0);
				waitpid(pid2, NULL,0);
				//printf("Childs completed!\n");
				
				printf("character-count: %d\n",charCountSum);
				printf("read-call-count: %d\n",Rcounter);
				printf("write-call-count: %d\n",Wcounter);
			}
		}
		
		double elapsedTime = 0.0;
		
		gettimeofday(&timeval2,NULL);
		
		elapsedTime = (timeval2.tv_usec - timeval1.tv_usec) / 1000.0;
		printf("--- TIME TAKEN TO EXECUTE: %f ms ---\n",elapsedTime);
		
		//printf("Your command: %s\n", input);
		//printf("Your first command: %s\n",input1);
		//printf("Your second command: %s\n",input2);
		
	}
	
}
