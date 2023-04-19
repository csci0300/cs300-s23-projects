#ifndef COLOR_HPP
#define COLOR_HPP

#include <iostream>
#include <string_view>

constexpr std::string_view RED = "\033[0;31m";
constexpr std::string_view YELLOW = "\033[0;33m";
constexpr std::string_view BLUE = "\033[0;34m";
constexpr std::string_view DIM = "\033[0;37m";

constexpr std::string_view NC = "\033[0m";  // No color

// accept anything that can be outputted via <<
template <typename T>
concept Printable = requires(std::ostream& os, T s) {
  os << s;
};

// print in color to the specified output stream
template <Printable Color, Printable... Args>
inline void print_color(std::ostream& os, const Color& color, Args&&... s) {
  os << color;
  // :o cpp is cool
  (os << ... << s);
  os << NC;
}

// Prints to std::cout the strings you provide. Appends a \n to the end!
template <Printable Color, Printable... Args>
inline void cout_color(const Color& color, Args&&... s) {
  print_color(std::cout, color, std::forward<Args>(s)..., '\n');
}

// Prints to std::cerr the strings you provide. Appends a \n to the end!
// We do this as a macro to allow for file:line error printing; unfortunately,
// std::source_location isn't implemented by most C++ compilers yet :(
#define cerr_color(color, ...)                                        \
  do {                                                                \
    print_color(std::cerr, color, "In function ", __FUNCTION__, " (", \
                __FILE__, ':', __LINE__, "): ", __VA_ARGS__, '\n');   \
  } while (0)

// Here's how you would do it in a C++ manner!
/*
 * template <Printable Color, Printable... Args>
 * inline void cerr_color(const Color &color, Args &&...s) {
 *      print_color(std::cerr, color, std::forward<Args>(s)..., '\n');
 * }
 */

// perror in color
template <Printable Color>
inline void perror_color(const Color& color, const char* s) {
  std::cerr << color;
  perror(s);
  std::cerr << NC;
}

#endif /* end of include guard */
