#include "config.hpp"

std::string ShardmasterConfig::print() {
  std::stringstream ss;
  ss << "Shardmaster configuration: \n";
  for (auto&& sc : this->servers) {
    ss << "- " << sc.server << ": ";
    for (auto&& s : sc.shards) {
      ss << s;
      if (s != sc.shards.back()) ss << ", ";
    }
    ss << '\n';
  }
  return ss.str();
}

std::optional<std::string> ShardmasterConfig::get_server(
    const std::string& key) {
  std::string key_uppercase = to_upper(key);
  auto it = std::find_if(servers.cbegin(), servers.cend(),
                         [&key_uppercase](auto&& sc) {
                           for (auto&& s : sc.shards) {
                             if (s.contains(key_uppercase)) return true;
                           }
                           return false;
                         });
  if (it == this->servers.cend()) {
    cerr_color(RED,
               "Shardmaster config does not contain any server responsible for "
               "the key!");
    return std::nullopt;
  }
  return it->server;
}

// TODO (optional): Implement any helper functions over a shardmaster
// configuration!
