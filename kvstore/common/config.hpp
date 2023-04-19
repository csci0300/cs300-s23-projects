#ifndef SHARDMASTER_CONFIG_HPP
#define SHARDMASTER_CONFIG_HPP

#include <algorithm>
#include <optional>
#include <string>
#include <vector>

#include "shard.hpp"

// Struct representing a server configuration, with its responsible shards.
struct ServerConfig {
  std::string server;
  std::vector<Shard> shards;
};

// Struct representing a Shardmaster's configuration. You may find this helpful
// to encapsulate operations over a configuration.
struct ShardmasterConfig {
  std::vector<ServerConfig> servers;

  // Pretty printing of server configuration
  std::string print();
  // Gets the server with the shard for the key.
  std::optional<std::string> get_server(const std::string& key);

  // TODO (optional): Implement any helper functions over a shardmaster
  // configuration!
};

#endif /* end of include guard */
