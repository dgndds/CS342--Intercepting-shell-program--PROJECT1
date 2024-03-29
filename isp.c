#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

char** getParams(char* str){
	char *param = strtok(str," ");
	char** result = NULL;
	int space = 0;
	
	while(param){
		result = realloc(result , sizeof(char*)*++space);
		
		result[space - 1] = param;
		
		param = strtok(NULL," ");
	}
	
	result = realloc(result,sizeof(char*)*(space + 1));
	result[space] = 0;
	
	return result;
}

int main(int argc, char *argv[]){
	
	char *input = malloc(sizeof(char) * 256);
	char *input1 = malloc(sizeof(char) * 256);
	char *input2 = malloc(sizeof(char) * 256);
	char *ignore = malloc(sizeof(char) * 256);
	char** params1 = NULL;
	char** params2 = NULL;
	
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
		
		//EXIT OPTION
		if(strcmp(input,"q") == 0){
			break;
		}
		
		//INIT AND CLEAR BUFFERS
		memset(input,0,150);
		memset(input1,0,150);
		memset(input2,0,150);
		memset(ignore,0,150);
	
		// READ USER INPUT
		printf("\nisp$ ");
		fgets(input,150,stdin);
		sscanf(input,"%[^\n]",input);
		
		if(strcmp(input,"\n") == 0){
			printf("INPUT FORMAT IS NOT VALID\n");
			continue;
		}
			
		//SCAN THE INPUT AND CHECK FORMAT
		int result = sscanf(input, "%[^|^\t^\n]|%[^|^\t^\n]|%[^|^\t^\n]", input1, input2,ignore);
		
		if(result > 2){
			printf("INPUT FORMAT IS NOT VALID\n");
			continue;
		}else if(result == 2){
			if(strcmp(input2,"")==0){
				printf("INPUT FORMAT IS NOT VALID\n");
				continue;
			}
		}else if(result == 1){
			if(strcmp(input1,"")==0){
				printf("INPUT FORMAT IS NOT VALID\n");
				continue;
			}
		}else if(result == 0){
			printf("INPUT FORMAT IS NOT VALID\n");
			continue;
		}
		
		//ANALYZE COMMANDSs
		
		if(strcmp(input1,"") != 0){
			params1 = getParams(input1);
		}
		
		if(strcmp(input2,"") != 0){
			params2 = getParams(input2);
		}
		
		//START TIMER
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
				execvp(params1[0],params1);
				break;
			}else{
				wait(NULL);
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
					dup2(fd[0],STDIN_FILENO);
					close(fd[1]);
					close(fd[0]);
					execvp(params2[0],params2);
				}
				
				//Wait Childs
				close(fd[0]);
				close(fd[1]);
				waitpid(pid1,NULL,0);
				waitpid(pid2, NULL,0);
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
				Rcounter++;
				
				while(charCount != 0){
					write(fd2[1],buffer,bufferSize);
					Wcounter++;
					charCountSum += charCount;
					charCount = read(fd1[0],buffer,bufferSize);
					Rcounter++;
				}
				
				close(fd1[0]);
				close(fd2[1]);	
				
				//Wait Childs
				waitpid(pid1,NULL,0);
				waitpid(pid2, NULL,0);
				
				printf("character-count: %d\n",charCountSum);
				printf("read-call-count: %d\n",Rcounter);
				printf("write-call-count: %d\n",Wcounter);
			}
		}
		
		//STOP TIMER AND CALCULATE ELAPSED TIME
		gettimeofday(&timeval2,NULL);
		
		double elapsedTime = 0.0;
		
 		elapsedTime = (timeval2.tv_sec - timeval1.tv_sec) * 1000.0 + (timeval2.tv_usec - timeval1.tv_usec) / 1000.0;
		printf("--- TIME TAKEN TO EXECUTE: %f ms ---\n",elapsedTime);
		
		//FREE MEMORY BEFORE LOSING REFERENCE
		if(params1 != NULL){free(params1); params1 = NULL;}
		if(params2 != NULL){free(params2); params2 = NULL;}
		
	}
	
	//FREE ALLOCD MEMORY
	free(input);
	free(input1);
	free(input2);
	free(ignore);
	
	return 0;	
}
