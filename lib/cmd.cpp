#include <cstring>
#include <sstream>
#include <string>
#include <vector>

#include "cmd.hpp"

std::string trimString(const std::string &str) {
  auto start = str.find_first_not_of(" \t\n\r");

  if (start == std::string::npos)
    return "";

  auto end = str.find_last_not_of(" \t\n\r");

  return str.substr(start, end - start + 1);
}

std::vector<std::string> splitString(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::istringstream iss(str);
  std::string token;

  while (std::getline(iss, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}

Cmd::Cmd(const char *cmdtext, char prefix) {
  std::vector<std::string> args = splitString(std::string(cmdtext), ' ');

  for (int i = 0; i < args.size(); i++) {
    args[i] = trimString(args[i]);
  }

  if (args[0][0] == prefix) {
    strncpy(this->name, args[0].c_str() + 1, sizeof(this->name) - 1);
    this->name[sizeof(this->name) - 1] = '\0';
  }
  args.erase(args.begin());

  for (std::string arg : args) {
    this->args.push_back((char *)arg.c_str());
  }
}

char *Cmd::join() {
  char buff[64];
  char *result = (char *)malloc(256 * sizeof(char));
  result[0] = '\0';

  for (int i = 0; i < this->args.size(); i++) {
    sprintf(buff, "%s ", this->args[i]);
    strcat(result, buff);
  }

  return result;
}
