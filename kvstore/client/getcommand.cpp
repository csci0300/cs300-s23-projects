#include "getcommand.hpp"

void GetCommand::handle(const std::string& s) {
  std::vector<std::string> tokens = split(s);
  if (tokens.size() != 1) {
    cerr_color(RED, "Missing key. ", usage());
    return;
  }

  auto res = this->client->Get(tokens[0]);
  if (!res) {
    return;
  }

  std::cout << "Got value: " << *res << '\n';
}

std::string GetCommand::name() const {
  return "get";
}

std::string GetCommand::params() const {
  return "<key>";
}

std::string GetCommand::description() const {
  return "Gets <key>'s value; prints an error if not found";
}
