#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

#define MAX_LINE 80 
#define HISTSIZE 10


char* trimwhitespace(char *str);

void checkamp(int *count, char **tokens, int* amp)
{
   if(*count == 0) return; 
   size_t strlength = strlen(tokens[*count - 1]);
   if((strlength == 1) && tokens[*count - 1][0] == '&')
   {
	*amp = 1; 
	tokens[*count - 1] = NULL; 
	*count = *count - 1; 
   } else {
	tokens[*count] = NULL; 
   }
}

int compare(char *str, char *another)
{
	char* copy = (char*)malloc(MAX_LINE);
	strncpy(copy, str, strlen(str));
	copy = trimwhitespace(copy);
	if(strcmp(copy, another) == 0){
	 free(copy);
	 return 0; 
	} else {
	 free(copy);
	 return -1;
	} 
}

void doexit(int count, const char** tokens)
{
	if(count == 1 && (compare(tokens[count - 1], "exit") == 0)) 
	{
	      exit(0); 
	}
}

int  stringTok(char* sentence, char** tokens, int *camp) {
	
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
        tokens[count] = NULL;
	checkamp(&count, tokens, camp);
        doexit(count, tokens);
	free(copy);
	return count;   
}

int histrecord(char *args, char** hist, int* histcounter);
void viewhist(char **hist, int histcounter);

char* histcheck(char *args, char** hist, int histcounter)
{	 
	char *runcmd = (char*)malloc(MAX_LINE);
	char *s;
	//args = trimwhitespace(args);
	if(strcmp(args, "!!") == 0)
	{
		if(*hist){
		   strncpy(runcmd, *(hist + ((histcounter - 1) % 10)),
			 strlen(*(hist + ((histcounter - 1) % 10)))); 
	           printf("%s\n", runcmd);
		   return runcmd; 
		} else 
		{
		   printf("No commands could be found!\n");
		   return NULL; 
		}
	}
	int val = atoi(args + 1); 
	if(*args != NULL && args[0] == '!' && val)
	{ 	
		if(val <= histcounter && val > (histcounter - HISTSIZE) )
		{
		   strncpy(runcmd, *(hist + ((val - 1) % 10)),
			 strlen(*(hist + ((val - 1) % 10))));
		   printf("%s\n", runcmd);
		   return runcmd;
		} else 
		{
		   printf("No such N%dth command\n", val);
		   return NULL; 
		}
	}
	strncpy(runcmd, args,strlen(args));
	return runcmd; 
	//int n =  histrecord(args, hist, histcounter);
	//return n;
}

char* trimwhitespace(char *str)
{
	char *end; 
	
	// Trim leading space 
	while(isspace((unsigned char)*str)) str++;
	
	if(*str == 0) // all spaces
	  return str; 
    // Trim tailing space	
	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end)) end--; 
	
	// Write new null terminator
	*(end + 1) = 0;
	
	return str; 
}

int histrecord(char *args, char** hist, int* histcounter)
{
	args = trimwhitespace(args);
	if(histcounter >= 0)
	{
		char *runcmd = histcheck(args, hist, *histcounter);
		
		if(runcmd == NULL) return 0;  

		int i = 0; 
		size_t len = strlen(args);
			
		i = *histcounter % HISTSIZE;
		
		strncpy(hist[i], args, len);
		hist[i][len] = '\0';
		
		strncpy(args, runcmd, strlen(runcmd));
		free(runcmd); 
	}if(strcmp(args, "history") == 0){ 
	      viewhist(hist, *histcounter);  
	      *histcounter = *histcounter + 1;  
	      return 0; 
	   
	} else 
	{
	      return 1; 
	}
}

void viewhist(char** hist, int histcounter)
{
	int k = 0;
	
	if(histcounter < HISTSIZE)
	{
	  	for(int i = histcounter; i >= 0; i--)
	  	{
			printf("%d.\t%s\n", i + 1, hist[i]);	
	  	}
	} else 
        {	// # of times to print remaining cmds after HISTSIZE
		int remain = (histcounter % HISTSIZE) + 1;
		
		for(int i = histcounter; i > histcounter - remain; i--)
			printf("%d.\t%s\n", i + 1, hist[i % HISTSIZE]);
		
		// print the rest of nine elements
        	for(int i = histcounter - remain; i >=  remain; i--){
			k = i % HISTSIZE; 
	        	printf("%d.\t%s\n", i + 1, hist[k]);
		}
        }	
}

char** initialize()
{
	char** hist = malloc(HISTSIZE * sizeof(char *));
	for(int i = 0; i < HISTSIZE; i++)
	{
		hist[i] = (char*)malloc(MAX_LINE * sizeof(char));
	}
	return hist; 
}

int main(void) {
	char args[MAX_LINE]; // command to be executed
	char *cmdargv[MAX_LINE]; // command tokens
	char **hist;
	hist = initialize();
	int should_run = 1, isamp; 
	
	int histcounter = 0; 

	do {	
        	isamp = 0;           // ampersand checker	
		printf("osh>");
		fflush(stdout); 
		fgets(args, MAX_LINE, stdin);
		
		if(histrecord(args, hist, &histcounter) == 0)
			continue; 

		int count = stringTok(args, cmdargv, &isamp);	
		
		histcounter++;
		int pid = fork(); 

		if(pid == 0) {
			 
			if(execvp(cmdargv[0], cmdargv) < 0) {
			  perror("Error:");
			}

		} else if (pid > 0) { 
		  if(isamp) {
			  wait(&pid);
		  }	
		} else {
			printf("Fork failed\n");
			exit(1); 
		} 
		printf("\n");
		
	} while(should_run); 
	
	return 0;
}

