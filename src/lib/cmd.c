#include "cmd.h"

char *cmdtrim(const char *string) {
  char *result = (char *)malloc((MAX_CMD_NAME + MAX_CMD_ARGS) * sizeof(char));
  char *buffer = (char *)malloc((MAX_CMD_NAME + MAX_CMD_ARGS) * sizeof(char));
  result[0] = '\0';
  buffer[0] = '\0';
  bool first = true;

  for (size_t i = 0; i < strlen(string); i++) {
    if (string[i] != ' ' && first)
      first = false;
    if (!first)
      strncat(buffer, &string[i], 1);
  }

  first = true;

  for (size_t i = strlen(buffer); i > 0; i--) {
    if (buffer[i] != ' ' && first)
      first = false;
    if (!first)
      strncat(result, &buffer[i], 1);
  }

  size_t length = strlen(result);
  for (size_t i = 0; i < length / 2; i++) {
    char c = result[i];
    result[i] = result[length - 1 - i];
    result[length - 1 - i] = c;
  }

  free(buffer);
  return result;
}

Cmd cmdparse(const char *string) {
  Cmd result;
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
