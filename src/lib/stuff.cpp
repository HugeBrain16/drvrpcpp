#include "stuff.hpp"
#include <cstring>

std::vector<std::string> parselist(const char *src, char sep) {
  std::vector<std::string> result;
  std::string buff;

  for (int i = 0; i < strlen(src); i++) {
    if (src[i] == sep && !buff.empty()) {
      result.push_back(buff);
      buff.clear();
    } else
      buff += src[i];
  }

  if (!buff.empty())
    result.push_back(buff);

  return result;
}

std::string tolist(std::vector<std::string> src, char sep) {
  std::string result;

  for (int i = 0; i < src.size(); i++) {
    result += src[i];

    if (i != (src.size() - 1))
      result += sep;
  }

  return result.c_str();
}

std::string appendlist(const char *src, const char *value, char sep) {
  std::vector list = parselist(src, sep);
  list.push_back(value);

  return tolist(list, sep);
}
