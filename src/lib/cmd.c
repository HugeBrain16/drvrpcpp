#include "cmd.h"

char *cmdtrim(const char *string) {
  while (*string == ' ') string++;

  char *result = (char*) malloc((MAX_CMD_NAME + MAX_CMD_ARGS) * sizeof(char));
  size_t end = strlen(string);

  while (end > 0 && string[end - 1] == ' ') end--;
  strncpy(result, string, end);
  result[end] = '\0';

  return result;
}

Cmd cmdparse(const char *string) {
  Cmd result;
  memset(&result, 0, sizeof(Cmd));
  char *str = cmdtrim(string);

  if (str[0] == '/') {
    size_t i = 1;

    while (str[i] != ' ' && i < strlen(str)) {
      result.name[i - 1] = str[i];
      i++;
    }
    result.name[i - 1] = '\0';

    size_t length = strlen(result.name) + 1;
    int p = 0;
    for (size_t i = length; i < strlen(str); i++) {
      result.args[p] = str[i];
      p++;
    }
    result.args[p] = '\0';
  }
  free(str);
  return result;
}
