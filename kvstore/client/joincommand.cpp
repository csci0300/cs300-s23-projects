#include "joincommand.hpp"

void JoinCommand::handle(const std::string& s) {
  std::vector<std::string> tokens = split(s);
  if (tokens.size() != 1) {
    cerr_color(RED, "Missing server address. ", usage());
    return;
  }
  // enforce valid address
  size_t idx = tokens[0].find(':');
  if (idx == std::string::npos || !is_number(tokens[0].substr(idx + 1))) {
    cerr_color(RED, "Invalid server address. ", usage());
    return;
  }

  auto res = this->client->Join(tokens[0]);
  if (!res) {
    cerr_color(RED, "Failed to send Join request to shardmaster.");
    return;
  }
}

std::string JoinCommand::name() const {
  return "join";
}

std::string JoinCommand::params() const {
  return "<address>";
}

std::string JoinCommand::description() const {
  return "tells the Shardmaster that the key-value server running on <address> "
         "(hostname:port) "
         "is available for shard distribution.";
}
