#include "path.h"

void restorePath(path *p){
	setenv("PATH", p->originalPath, 1);
	exit(0);
}

void appendPath(path *p, const char *pathname){
	if(p->count < MAX_PATH){
		p->pathList[p->count++] = strdup(pathname);
		setenv("PATH", p->originalPath, 0 );
	}
	else{
		fprintf(stderr, "cannot add more to path");
	}
}

void displayPath(path *p){
	for(int i = 0; i<p->count; i++){
		printf("%s", p->pathList[i]);
		if(i<p->count -1){
			printf(":");
		}
	}
	printf("\n");
}

void removePath(path *p, const char *pathName){
	for (int i = 0; i < p->count; i++){
		if(strcmp(p->pathList[i], pathName)==0){
			free(p->pathList[i]);
			for(int j = i; j< p->count-1; j++){
				p->pathList[j] = p->pathList[j+1];
			}
			p->count--;
			setenv("PATH", p->originalPath, 0);
			break;
		}
	}
}
