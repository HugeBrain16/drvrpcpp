#include <string>
#include <vector>

std::vector<std::string> parselist(const char *src, char sep = ',');
std::string tolist(std::vector<std::string> src, char sep = ',');
std::string appendlist(const char *src, const char *value, char sep = ',');
