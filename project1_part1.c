#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#define MAX_LINE 80 


int  stringTok(char* sentence, char** tokens) {
	
	const char* delim = " \r\n"; 
	int count = 0; 
	
	// make copy of sentence for strtok 
	size_t len = strlen(sentence);
	char *copy = malloc(len+1); 
	if(!copy) return 0; // if copy null, return 0 
	strncpy(copy, sentence, len); 
	
	copy[len] = '\0'; 

	// Allocate and copy tokens 
	for(char* word = strtok(copy, delim); 
	    word; 
	    word = strtok(NULL, delim)) 
	{	
	    size_t len = strlen(word); 
	    tokens[count] = malloc(len+1);
	    if(!tokens[count]) break; 
            strncpy(tokens[count], word, len); 
            tokens[count][len] = '\0'; 
            count++; 
	} 
        size_t strlenth = strlen(tokens[count - 1]);
	//if(count > 1) {
	//   tokens[count - 1][strlenth -1] = '\0'; 
	//}
	tokens[count] = NULL; 
	return count;   
}

int main(void) {
	char args[MAX_LINE]; // command to be executed
	char *cmdargv[MAX_LINE]; // command tokens
	
	int should_run = 1; 

	do {
		printf("osh>");
		fflush(stdout); 
		fgets(args, MAX_LINE, stdin);
		//fflush(stdout); 
		
		printf("args:<%s>\n", args);
		
		int count = stringTok(args, cmdargv);
		for(int i = 0; i < count; i++) {
			printf("len %ld\n", strlen(cmdargv[i]));
			printf("content: %s\n", cmdargv[i]);
			printf("last elememnt %d\n", cmdargv[i][strlen(cmdargv[i]) - 1]);
		}
	        printf("last member:%s", cmdargv[count]);	
		//printf("cmdargv:%s\n", cmdargv[0]);
		//printf("cmdargc:%d\n", cmdargv[0][strlen(cmdargv[0]) - 1]);
		int pid = fork(); 
		
		char *cmd = "ls";
		char *argv[3];
		argv[0] = "ls";
		argv[1] = "-la";	
		
		if(pid == 0) {
			execvp(cmd, argv); 
			if(execvp(cmd, argv) < 0) {
			  //printf("Error Executing Command\n");
			  perror("Error:");
			}
		} else if (pid > 0) { 
			 // if '&' {
			 // wait(&pid)
			 //}	
		} else {
			printf("Fork failed\n");
			exit(1); 
		} 
		printf("\n"); 
	} while(should_run); 
	return 0;
}
