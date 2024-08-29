#include "main.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_COMMAND_LENGHT 512
#define num_buitins 5
#define MAX_TOKENS 256
#define TOKEN_SIZE 64

int signals[] = {SIGINT, SIGTERM};
char* builtins[] = {"cd", "exit", "path", "myhistory"};
int exit_status;
history History;
path p;

int (*builtin_func[])(char**) = {
	&launch_cd,
	&launch_exit,
	&launch_path,
	&launch_history
};


int launch_cd(char** args){
	if(args[2] != NULL){
		printf("Usage: cd <dirname> \n");
		return 1;
	}

	if(args[1] == NULL){
		chdir("/home");
	}

	else if(chdir(args[1]) == -1){
		perror("chdir");
	}
	return 1;
}

int launch_exit(char** args){
	//exit call here
	pid_t parent = getppid();
	kill(parent, SIGKILL);
	exit_status = 1;
	exit(1);

}

int launch_path(char ** args){
	//path call here
    if (args[1] != NULL)
    {
        //appending
        if(strcmp(args[1], "+")==0)
        {
            appendPath(&p, args[2]);
        }
        //removing
        else if (strcmp(args[1], "-") ==0)
        {
           removePath(&p, args[2]);
        }
    }
    else
    {
        //if nothing but path is entered
        displayPath(&p);
    }
    return 1;
}

int launch_history(char** args){
	if(strcmp(args[1], "-c")==0){
		clearHist(&History);
	}
	else if(strcmp(args[1], "-e")==0){
		for(int i=0;i<History.count; i++){
			if(i == atoi(args[2])){
				execute(History.history[i]);
			}
		}
	}
	else if(args[1] == NULL){
		printHist(&History);
	}
}



typedef struct {
    char **tokens;
    size_t count;
} TokenArray;

typedef struct{
	char* args[64];
	char *input;
	char *output;
	int append;
	} Command;


 void handleSig(int sig){
	printf("\n");
}
TokenArray tokenize(const char *string, const char *pattern) {
	regex_t regex;
	regmatch_t match;
	TokenArray result;
   	char token[TOKEN_SIZE];
   	size_t token_capacity = 10; // Initial capacity for token storage

   	// Allocate initial toktokens.count
	result.tokens = malloc(token_capacity * sizeof(char*));
   	result.count = 0;

   	if (result.tokens == NULL) {
   	    fprintf(stderr, "Memory allocation failed\n");
   	    exit(1);
   	}

   	// Compile the regular expression
   	if (regcomp(&regex, pattern, REG_EXTENDED) != 0) {
   	    fprintf(stderr, "Could not compile regex\n");
   	    exit(1);
   	}

   	const char *p = string;
   	while (regexec(&regex, p, 1, &match, 0) == 0) {
   	    int len = match.rm_eo - match.rm_so;
   	    if (len >= TOKEN_SIZE) len = TOKEN_SIZE - 1;

   	    // Allocate memory for the token
   	    char *new_token = malloc(len + 1);
   	    if (new_token == NULL) {
   	        fprintf(stderr, "Memory allocation failed\n");
   	        exit(1);
   	    }

   	    strncpy(new_token, p + match.rm_so, len);
   	    new_token[len] = '\0';

   	    // Check if we need to expand the token array
   	    if (result.count >= token_capacity) {
   	        token_capacity *= 2;
   	        result.tokens = realloc(result.tokens, token_capacity * sizeof(char*));
   	        if (result.tokens == NULL) {
   	            fprintf(stderr, "Memory reallocation failed\n");
   	            exit(1);
   	        }
   	    }

   	    result.tokens[result.count++] = new_token;

   	    p += match.rm_eo;
   	    if (*p == '\0') break;
   	}

	result.tokens[result.count++] = NULL;
   	regfree(&regex);
   	return result;
}

Command *parse_command(char *cmd) {
	Command *command = malloc(sizeof(Command));
	int argc = 0;
	command->input = NULL;
	command->output = NULL;
   	command->append = 0;

   	char *token = strtok(cmd, " \t\n\a\r");
   	while (token != NULL) {
   	    if (strcmp(token, "<") == 0) {
   	        token = strtok(NULL, " \t\n\a\r");
   	        command->input = strdup(token);
   	    } else if (strcmp(token, ">") == 0) {
   	        token = strtok(NULL, " \t\n\a\r");
   	        command->output = strdup(token);
   	        command->append = 0;
   	    } else {
   	        command->args[argc++] = strdup(token);
   	    }
   	    token = strtok(NULL, " \t\n\a\r");
   	}
   	command->args[argc] = NULL;
   	return command;
}

int execute(char ** args){
	if(args[0] == NULL){
		return 1;
	}

	for(int i=0; i<BUILTIN_NUM; i++){
		if(strcmp(args[0], builtins[i]) ==0){
			return(*builtin_func[i])(args);
		}
	}
	return launchProgram(args);
}

int launchProgram(char** args){
	if(execvp(args[0], args)==-1){
		perror("shell");
		exit(EXIT_FAILURE);
	}
	return 0;
}
//Dumpster fire attempts
/*char** splitLine(char *input, char* delims){			//string tokenizer, tokenizes the input of the user based on whitespace characters into seperate arguments to be used by the builtins
	int bufSize = TOK_BUFSIZE;
	int position = 0;
	char** tokens = malloc(TOK_BUFSIZE * sizeof(char*));
	char * token;
	if(!tokens){
		printf("allocation error");
		exit(EXIT_FAILURE);
	}
	token = strtok(input, delims);
	while(token != NULL){
		tokens[position] = token;
		position++;
		if(position >=bufSize){
			bufSize += TOK_BUFSIZE;
			tokens = realloc(tokens, bufSize * sizeof(char*));
			if(!tokens){
				printf("allocation error");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, delims);
	}
	tokens[position] = NULL;
	return tokens;
}

/*char** split_line(const char* input, char delimiter, int input_length)
{
	//# of delimiters
	int count = 0;
	for(int i = 0; i < input_length; i++)
	{
		if(input[i] == delimiter)
		{
			count++;
		}
	}


	
	//Alloc memory for pointers to tokens
	char** tokens = malloc((count + 2) * sizeof(char*));
	if(!tokens) 
	{
		return NULL;
	}
	
	int index = 0; //index for tokens
	const char* token_start = input; //start @ input
	int len = 0; //length of currToken
	for (int i = 0; i <= input_length; i++)
	{
		if(i == input_length || input[i] == delimiter)
		{
			if(len > 0)
			{
				tokens[index] = malloc(len+1);
				if(tokens[index])
				{
					strncpy(tokens[index], token_start, len);
					tokens[index][len] = '\0';
					index++;
				}
			}
			token_start = input + i + 1;
			len = 0;
		}
		else
		{
			len++;
		}
	}
	tokens[index] = NULL;
	return tokens;
}*/
/*void implementPipe(char **cmd1, char **cmd2, char **cmd3){
	int pipe1[2], pipe2[2];
	if(pipe(pipe1) == -1 || pipe(pipe2) == -1){
		perror("could not use");
		exit(EXIT_FAILURE);
	}
	exeForPipe(cmd1, pipe1);
	exeForPipe(cmd2, pipe1);
	close(pipe1[0]);
	close(pipe[1]);
	exeForPipe(cmd3, pipe2);
	close(pipe2[0]);
	close(pipe2[1]);
}*/

int loop()
{
	//User and host variables
	char host[32], user[64];
	int hostCheck, userCheck;
	char input[MAX_COMMAND_LENGHT + 1];
	int exitCheck = 0;
	int cmd_count=0;


	
	
	//Get host name + checker
	hostCheck = gethostname(host, sizeof(host));
	if(hostCheck == -1)
	{
		perror("HostName");
		exit(1);
	}
	
	//Get username + checker
	userCheck = getlogin_r(user, sizeof(user));
	if(userCheck == -1)
	{
		perror("UserName");
		exit(1);
	}
	
	
	while(1)
	{
		printf("%s@%s> ", host, user);
		fflush(stdout);
		char * cmd[64];
		char* temp;
		char * temp1;
		char* temp2[64];
		char* temp3;
		int cmd_count = 0;
		int cc2=0;
		fgets(input, MAX_COMMAND_LENGHT + 1, stdin);
		//char** args = splitLine(str);
		temp1 = strtok(input, ";");
		while (temp1 != NULL && cmd_count < 64) {
			temp2[cc2++] = strdup(temp1);
			temp1 = strtok(NULL, ";");
		}
		temp2[cc2++] = NULL;
		for(int k=0;temp2[k]!=NULL;k++){
			temp3 = strdup(temp2[k]);
			temp = strtok(temp3, "|");
			while (temp != NULL && cmd_count < 64) {
				cmd[cmd_count++] = strdup(temp);
				temp = strtok(NULL, "|");
			}
			int pipes[2];
			int lastinp = 0;
	
			Command * commands[64];
			for(int i = 0; i<cmd_count; i++){
				commands[i] = parse_command(cmd[i]);
			}
	
			pid_t pid;
			int status;
			for(int i= 0; i< cmd_count; i++){
				if(i < cmd_count-1){
					if(pipe(pipes)<0){
						perror("Couldn't pipe");
						exit(EXIT_FAILURE);
					}
				}
				pid_t pid = fork();
				if(pid == 0){
					//pid_t child = getpid();
					//setpgid(child,child);	
					//pid_t gpid = tcsetpgrp(0,child);
					for(int s=0;s<2;s++){
						if(signal(signals[s], SIG_DFL)==SIG_ERR){
							fprintf(stderr, "could not set default signal handler %d\n", signals[s]);
						}
					}
				        if (commands[i]->input) { // Input redirection
						int fin = open(commands[i]->input, O_RDONLY);
						if (fin < 0) {
							perror("open input");
							exit(EXIT_FAILURE);
						}
						dup2(fin, STDIN_FILENO);
						close(fin);
					} else if (lastinp != 0) { // Pipe input
						dup2(lastinp, STDIN_FILENO);
						close(lastinp);
					}
	
					if (commands[i]->output) { // Output redirection
						int flags = O_WRONLY | O_CREAT | (commands[i]->append ? O_APPEND : O_TRUNC);
						int fout = open(commands[i]->output, flags, 0644);
						if (fout < 0) {
							perror("open output");
							exit(EXIT_FAILURE);
						}
						dup2(fout, STDOUT_FILENO);
						close(fout);
					} else if (i < cmd_count -1) { // Pipe output
					dup2(pipes[1], STDOUT_FILENO);
					}
					execute(commands[i]->args);
					add(&History, commands[i]->args);
				}
		
				if(lastinp != 0){
					close(lastinp);
				}
				if(i<cmd_count-1){
					close(pipes[1]);
					lastinp = pipes[0];
				}
				
			}
			while(wait(&status)>0);
			/*
			for(int i = 0; tokens.tokens[i] != NULL; i++)
			{
				
				printf("%s", tokens.tokens[i]);
				char temp[TOKEN_SIZE];
				strcpy(temp, tokens.tokens[i]);
				TokenArray cmd = tokenize(temp, TOK_DELIM);
				for(int j = 0; cmd.tokens[j] !=NULL; j++){
					printf("%s\n", cmd.tokens[j]);
				}
				execute(cmd.tokens);
			}*/
		}
	}
	return 0;
}


int batch (char* fileName)
{
	printf("Batch started\n");
	FILE* fp = fopen(fileName, "r");
	char input [512];
	int length = 0;
	printf("Gets to here\n");
	while(fgets(input, 512, fp))
	{
		printf("%s", input);
		char * cmd[64];
		char* temp;
		char * temp1;
		char* temp2[64];
		char* temp3;
		int cmd_count = 0;
		int cc2=0;
		temp1 = strtok(input, ";");
		while (temp1 != NULL && cmd_count < 64) {
			temp2[cc2++] = strdup(temp1);
			temp1 = strtok(NULL, ";");
		}
		temp2[cc2++] = NULL;
		for(int k=0;temp2[k]!=NULL;k++){
			temp3 = strdup(temp2[k]);
			temp = strtok(temp3, "|");
			while (temp != NULL && cmd_count < 64) {
				cmd[cmd_count++] = strdup(temp);
				temp = strtok(NULL, "|");
			}
			int pipes[2];
			int lastinp = 0;
	
			Command * commands[64];
			for(int i = 0; i<cmd_count; i++){
				commands[i] = parse_command(cmd[i]);
			}
	
			pid_t pid;
			int status;
			for(int i= 0; i< cmd_count; i++){
				if(i < cmd_count-1){
					if(pipe(pipes)<0){
						perror("Couldn't pipe");
						exit(EXIT_FAILURE);
					}
				}
				pid_t pid = fork();
				if(pid == 0){
					for(int s=0;s<2;s++){
						if(signal(signals[s], SIG_DFL)==SIG_ERR){
							fprintf(stderr, "could not set default signal handler %d\n", signals[s]);
						}
					}
				        if (commands[i]->input) { // Input redirection
						int fin = open(commands[i]->input, O_RDONLY);
						if (fin < 0) {
							perror("open input");
							exit(EXIT_FAILURE);
						}
						dup2(fin, STDIN_FILENO);
						close(fin);
					} else if (lastinp != 0) { // Pipe input
						dup2(lastinp, STDIN_FILENO);
						close(lastinp);
					}
	
					if (commands[i]->output) { // Output redirection
						int flags = O_WRONLY | O_CREAT | (commands[i]->append ? O_APPEND : O_TRUNC);
						int fout = open(commands[i]->output, flags, 0644);
						if (fout < 0) {
							perror("open output");
							exit(EXIT_FAILURE);
						}
						dup2(fout, STDOUT_FILENO);
						close(fout);
					} else if (i < cmd_count -1) { // Pipe output
					dup2(pipes[1], STDOUT_FILENO);
					}
					execute(commands[i]->args);
				}
		
				if(lastinp != 0){
					close(lastinp);
				}
				if(i<cmd_count-1){
					close(pipes[1]);
					lastinp = pipes[0];
				}
				
			}
			while(wait(&status)>0);
		}
	}
	fclose(fp);
	return 0;
}
int main(int argc, char* argv[]){
	char input[MAX_COMMAND_LENGHT];
	char* args[MAX_ARGUMENTS];
	int exit = 0;
	
	struct sigaction act;
	act.sa_handler = handleSig;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;

	for(int i=0;i<2;i++){
		if(sigaction(signals[i], &act, NULL)){
			fprintf(stderr,"cannot ignore signal %d\n", signals[i]);
		}
	}


	if(argc == 1 ){
		//char* input;
		//char** args;
		//int status;
		//char* temp;

		//input = readLine();
		//strcpy(temp, input);
		//args = splitLine(input);
		
		loop();
	}
	else if(argc == 2)
	{
		batch(argv[1]);
	}

	return exit;
}
