#include <array>

#define MAX_CMD_ARGS 64

class Cmd {
public:
  char name[32];
  std::array<char*, MAX_CMD_ARGS> args;

  Cmd(const char *cmdtext, char prefix = '/');
};
