#include "history.h"
void add(history *History, char **cmd){
	if(History->count < HISTORY_LIMIT){
		int i = 0;
		History->history[History->count] = malloc(sizeof(cmd));
		for(i=0; cmd[i] != NULL;i++){
			strcpy(History->history[History->count][i], cmd[i]);
		}
		History->history[History->count][i] = NULL;
		History->count++;
		return;
	}
	else{
		int i;
		for(i = 0; i < HISTORY_LIMIT - 1; i++){
			for(int j=0;History->history[i][j] != NULL; j++){
				strcpy(History->history[i][j], History->history[i+1][j]);
			}
		}
		for(i=0; cmd[i] != NULL;i++){
			strcpy(History->history[HISTORY_LIMIT-1][i], cmd[i]);
		}
	}
	return;
}

void printHist(history *History){
	printf("History:\n");
	int i = 0;
	for(i = 0; i < History->count; i++){
			for(int j=0;History->history[i][j] != NULL; j++){
				printf("%s\n", History->history[i][j]);
			}
		printf("\n");
		}
}

void clearHist(history *History){
  for(int i = 0; i < History->count; i++){
	  free(History->history[i]);
  }
	  History->count = 0;
	return;
}
