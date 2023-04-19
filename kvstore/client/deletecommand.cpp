#include "deletecommand.hpp"

void DeleteCommand::handle(const std::string& s) {
  std::vector<std::string> tokens = split(s);
  if (tokens.size() != 1) {
    cerr_color(RED, "Missing key. ", usage());
    return;
  }

  auto res = this->client->Delete(tokens[0]);
  if (!res) {
    return;
  }

  std::cout << "Got value: " << *res << '\n';
}

std::string DeleteCommand::name() const {
  return "delete";
}

std::string DeleteCommand::params() const {
  return "<key>";
}

std::string DeleteCommand::description() const {
  return "Deletes <key>, and gets its last value";
}
