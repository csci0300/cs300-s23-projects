#include "utils.hpp"

std::vector<std::string> split(const std::string& s, char delim) {
  std::vector<std::string> res;

  std::istringstream iss(s);
  std::string item;
  while (std::getline(iss, item, delim)) {
    if (!item.empty()) res.push_back(item);
  }

  return res;
}

bool is_number(const std::string& s) {
  return !s.empty() && std::accumulate(s.cbegin(), s.cend(), true,
                                       [](bool a, unsigned char c) {
                                         return a && std::isdigit(c);
                                       });
}

std::string to_upper(const std::string& s) {
  std::string res = s;
  std::transform(res.begin(), res.end(), res.begin(),
                 [](unsigned char c) { return std::toupper(c); });
  return res;
}

std::string to_lower(const std::string& s) {
  std::string res = s;
  std::transform(res.begin(), res.end(), res.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return res;
}
