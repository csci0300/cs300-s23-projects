#include "repl.hpp"

void Repl::add_command(ReplCommand& command) {
  this->commands.push_back(&command);
}

void Repl::run() {
  std::cout << "Type 'help' for a list of commands.\n";
  std::string line;
  while (std::getline(std::cin, line)) {
    try {
      process_line(line);
    } catch (std::exception& e) {
      cerr_color(RED, "Method failed with error message: ", e.what());
    }
  }
}

void Repl::process_line(const std::string& line) {
  size_t spaceIdx = line.find(' ');
  std::string cmdName = line.substr(0, spaceIdx);
  std::string remaining = "";
  if (spaceIdx != std::string::npos) {
    remaining = line.substr(spaceIdx + 1);
  }

  if (!line.empty()) {
    if (cmdName == "help") {
      std::cout << help() << "\n";
    } else if (auto cmd = std::find_if(
                   commands.begin(), commands.end(),
                   [cmdName](auto&& c) { return c->name() == cmdName; });
               cmd != commands.end()) {
      (*cmd)->handle(remaining);
    } else {
      cerr_color(YELLOW, "Invalid command.");
      std::cout << help() << "\n";
    }
  }
}

std::string Repl::help() {
  std::string res = "Available commands:\n";
  for (auto& cmd : commands) {
    res += "- " + cmd->help() + '\n';
  }
  return res;
}
