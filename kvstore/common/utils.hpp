#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <cctype>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

// Random utils. Might break up into different files if the scope blossoms.

static constexpr auto hasher = std::hash<std::string>{};
// Helper function to hash a string.
inline size_t hash(const std::string& str) {
  return hasher(str);
}

// Helper function to split strings by whitespace. gotta love cpp for not having
// this lmao
std::vector<std::string> split(const std::string& s, char delim = ' ');

// Checks if a string is a (positive) number
bool is_number(const std::string& s);

// Convert a string to upper/lower case, because cpp doesn't have that
// functionality yet... this is pretty fragile, it only works on ASCII
// characters right now (sorry Unicode!)
std::string to_upper(const std::string& s);
std::string to_lower(const std::string& s);

#endif /* end of include guard */
