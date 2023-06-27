#include <cstring>
#include <string>
#include <vector>
#include <sstream>

#include "cmd.hpp"

std::string trimString(const std::string& str) {
  auto start = str.find_first_not_of(" \t\n\r");

  if (start == std::string::npos)
    return "";

  auto end = str.find_last_not_of(" \t\n\r");

  return str.substr(start, end - start + 1);
}

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;

    while (std::getline(iss, token, delimiter)) {
      tokens.push_back(token);
    }

    return tokens;
}

Cmd::Cmd(const char *cmdtext, char prefix) {
  std::vector args = splitString(std::string(cmdtext), ' ');

  for(int i; i < args.size(); i++) {
    args[i] = trimString(args[i]);
  }

  if(args[0][0] == prefix) {
    for(int i = 1; i < args[0].length(); i++)
      this->name[i-1] = args[0][i];
    this->name[args[0].length()-1] = '\0';
  }
  args.erase(args.begin());

  for(int i; i < args.size(); i++)
    strcpy(this->args[i], args[i].c_str());
}
