#include "movecommand.hpp"

void MoveCommand::handle(const std::string& s) {
  std::vector<std::string> tokens = split(s);
  // Check that >=1 shard is given
  if (tokens.size() < 3) {
    cerr_color(RED, "Missing server address, or at least 1 shard. ", usage());
    return;
  }
  // Check that there are no partial shards
  if (tokens.size() % 2 == 0) {
    cerr_color(RED, "Ill-formed shards. ", usage());
  }

  // Enforce valid address
  size_t idx = tokens[0].find(':');
  if (idx == std::string::npos || !is_number(tokens[0].substr(idx + 1))) {
    cerr_color(RED, "Ill-formed address. ", usage());
    return;
  }

  // Enforce that each shard's bounds are valid, and have the same granularity.
  std::vector<Shard> shards((tokens.size() - 1) / 2);
  for (size_t i = 1; i < tokens.size(); i += 2) {
    if (!is_valid(tokens[i]) || !is_valid(tokens[i + 1])) {
      cerr_color(
          RED,
          "Invalid shard boundaries; valid characters (case-insensitive): ",
          std::quoted(VALID_CHARS), ".");
      return;
    }
    if (tokens[i].size() != tokens[i + 1].size()) {
      cerr_color(RED, "Invalid shard boundaries; must be same granularity. ",
                 usage());
      return;
    }
    shards[i / 2] = {to_upper(tokens[i]), to_upper(tokens[i + 1])};
  }

  auto res = this->client->Move(tokens[0], shards);
  if (!res) {
    cerr_color(RED, "Failed to send Move request to shardmaster.");
    return;
  }
}

std::string MoveCommand::name() const {
  return "move";
}

std::string MoveCommand::params() const {
  return "<address> <s1 lower> <s1 upper> ...";
}

std::string MoveCommand::description() const {
  return "moves shards s1, ..., sn to the key-value server running on "
         "<address> (hostname:port)";
}
