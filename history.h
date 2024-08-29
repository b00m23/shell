#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


#define MAX_ARGUMENTS 120
#define MAX_COMMAND_LENGTH 100
#define HISTORY_LIMIT 20


typedef struct{
	char **history[HISTORY_LIMIT];
	int count;
}history;



void add(history *History, char **cmd);
void printHist(history *History);

void clearHist(history *History);
