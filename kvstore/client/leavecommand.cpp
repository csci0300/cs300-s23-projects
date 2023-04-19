#include "leavecommand.hpp"

void LeaveCommand::handle(const std::string& s) {
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

  auto res = this->client->Leave(tokens[0]);
  if (!res) {
    cerr_color(RED, "Failed to send Leave request to shardmaster.");
    return;
  }
}

std::string LeaveCommand::name() const {
  return "leave";
}

std::string LeaveCommand::params() const {
  return "<address>";
}

std::string LeaveCommand::description() const {
  return "tells the Shardmaster that the key-value server on the specified "
         "address "
         "(hostname:port) is no longer available for shard distribution.";
}
