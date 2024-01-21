#ifndef CMD_H
#define CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 16
#define MAX_ARGS 256

typedef struct Cmd {
  char name[MAX_NAME];
  char args[MAX_ARGS];
} Cmd;

char *cmdtrim(const char *string);
Cmd cmdparse(const char *string);

#ifdef __cplusplus
}
#endif

#endif // CMD_H
