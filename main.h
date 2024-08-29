#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <regex.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include "cd.h"
#include "path.h"
#include "history.h"

#define MAX_ARGUMENTS 120
#define BUILTIN_NUM 4
#define TOK_BUFSIZE  64
#define TOK_DELIM  "[^ \t\r\n\a]+"
#define PIPE_REDIRECT "([^|]+)"

int execute(char ** args);
int launchProgram(char** args);
int launch_cd(char** args);
int launch_history(char** args);
int launch_exit(char** args);
int launch_path(char** args);
int loop();
int batch(char* fileName);
