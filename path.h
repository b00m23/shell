#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH 1024
typedef struct{
	char originalPath[1024];
	char *pathList[1024];
	int count;
}path;
void restorePath(path *p);
void appendPath(path *p, const char *pathname);
void displayPath(path *p);
void removePath(path *p, const char * pathname);
