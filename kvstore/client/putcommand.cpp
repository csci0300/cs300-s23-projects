#include "putcommand.hpp"

void PutCommand::handle(const std::string& s) {
  std::vector<std::string> tokens = split(s);
  if (tokens.size() != 2) {
    cerr_color(RED, "Missing key and/or value. ", usage());
    return;
  }

  auto res = this->client->Put(tokens[0], tokens[1]);
  if (!res) {
    return;
  }
}

std::string PutCommand::name() const {
  return "put";
}

std::string PutCommand::params() const {
  return "<key> <value>";
}

std::string PutCommand::description() const {
  return "Sets <key> to <value>";
}
