#include <vector>

class Cmd {
public:
  char name[32];
  std::vector<char *> args;

  Cmd(const char *cmdtext, char prefix = '/');
  char *join();
};
