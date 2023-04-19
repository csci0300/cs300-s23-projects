#include "appendcommand.hpp"

void AppendCommand::handle(const std::string& s) {
  std::vector<std::string> tokens = split(s);
  if (tokens.size() != 2) {
    cerr_color(RED, "Missing key and/or value. ", usage());
    return;
  }

  auto res = this->client->Append(tokens[0], tokens[1]);
  if (!res) {
    return;
  }
}

std::string AppendCommand::name() const {
  return "append";
}

std::string AppendCommand::params() const {
  return "<key> <value>";
}

std::string AppendCommand::description() const {
  return "Appends <value> to <key>; creates <key> if does not exist";
}
