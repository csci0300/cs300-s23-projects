#ifndef REPL_HPP
#define REPL_HPP

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "common/color.hpp"
#include "replcommand.hpp"

class Repl {
  // Ideally, should make copy assignment/constructors deleted, but can deal w/
  // later
 public:
  void add_command(ReplCommand& command);
  void run();

 private:
  std::vector<ReplCommand*> commands;

  void process_line(const std::string& line);
  std::string help();
};

#endif /* end of include guard */
