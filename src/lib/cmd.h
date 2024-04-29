#ifndef CMD_H
#define CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_NAME 16
#define MAX_CMD_ARGS 256

typedef struct Cmd {
  char name[MAX_CMD_NAME];
  char args[MAX_CMD_ARGS];
} Cmd;

char *cmdtrim(const char *string);
Cmd cmdparse(const char *string);

#ifdef __cplusplus
}
#endif

#endif // CMD_H
