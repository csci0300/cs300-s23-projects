#include "printcommand.hpp"

void PrintCommand::handle(const std::string& s) {
  std::vector<std::string> tokens = split(s);
  if (tokens.size() != 1) {
    cerr_color(RED, "Missing print type. ", usage());
    return;
  }

  if (to_lower(tokens[0]) == "store") {
    auto res = this->server->all_kvpairs();
    std::cout << "Key-value pairs:" << std::endl;
    for (auto& [k, v] : res) std::cout << "\t" << k << ": " << v << std::endl;
  } else if (to_lower(tokens[0]) == "config") {
    auto res = this->server->get_config();
    res.print();
  } else {
    cerr_color(RED, "Print type must be either \"store\" or \"config\".");
  }
}

std::string PrintCommand::name() const {
  return "print";
}

std::string PrintCommand::params() const {
  return "<store|config>";
}

std::string PrintCommand::description() const {
  return "Prints either the internal store contents, or the shardmaster "
         "configuration.";
}
