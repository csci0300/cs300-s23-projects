#include "multiputcommand.hpp"

void MultiPutCommand::handle(const std::string& s) {
  std::vector<std::string> tokens = split(s);
  if (tokens.size() < 2 || tokens.size() % 2) {
    cerr_color(RED, "Missing keys and/or values. ", usage());
    return;
  }

  std::vector<std::string> keys(tokens.size() / 2), values(tokens.size() / 2);
  for (size_t i = 0; i < tokens.size(); i += 2) {
    keys[i / 2] = tokens[i];
    values[i / 2] = tokens[i + 1];
  }

  auto res = this->client->MultiPut(keys, values);
  if (!res) {
    return;
  }
}

std::string MultiPutCommand::name() const {
  return "multiput";
}

std::string MultiPutCommand::params() const {
  return "<key1> <value1> ...";
}

std::string MultiPutCommand::description() const {
  return "Sets the values of <key1>, ..., <keyn> to <value1>, ..., <valuen>";
}
