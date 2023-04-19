#include "multigetcommand.hpp"

void MultiGetCommand::handle(const std::string& s) {
  std::vector<std::string> tokens = split(s);
  if (tokens.size() < 1) {
    cerr_color(RED, "Missing keys. ", usage());
    return;
  }

  auto res = this->client->MultiGet(tokens);
  if (!res) {
    return;
  }

  std::cout << "Got values: ";
  for (auto&& v : res.value()) std::cout << v << " ";
  std::cout << "\n";
}

std::string MultiGetCommand::name() const {
  return "multiget";
}

std::string MultiGetCommand::params() const {
  return "<key1> ...";
}

std::string MultiGetCommand::description() const {
  return "Gets the values of <key1>, ..., <keyn>; prints an error if not any "
         "are not found";
}
