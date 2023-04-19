#include "shardkv_client.hpp"

std::optional<std::string> ShardKvClient::Get(const std::string& key) {
  // Query shardmaster for config
  auto config = this->Query();
  if (!config) return std::nullopt;

  // find responsible server in config
  std::optional<std::string> server = config->get_server(key);
  // Here (and later) we can re-use logic from the simple client! woohoo code
  // reuse. I believe object creation here is on the stack, so it should be
  // almost free (minus string copying cost)
  if (!server) return std::nullopt;

  return SimpleClient{*server}.Get(key);
}

bool ShardKvClient::Put(const std::string& key, const std::string& value) {
  // Query shardmaster for config
  auto config = this->Query();
  if (!config) return false;

  // find responsible server in config, then make Put request
  std::optional<std::string> server = config->get_server(key);
  if (!server) return false;
  return SimpleClient{*server}.Put(key, value);
}

bool ShardKvClient::Append(const std::string& key, const std::string& value) {
  // Query shardmaster for config
  auto config = this->Query();
  if (!config) return false;

  // find responsible server in config, then make Append request
  std::optional<std::string> server = config->get_server(key);
  if (!server) return false;
  return SimpleClient{*server}.Append(key, value);
}

std::optional<std::string> ShardKvClient::Delete(const std::string& key) {
  // Query shardmaster for config
  auto config = this->Query();
  if (!config) return std::nullopt;

  // find responsible server in config, then make Delete request
  std::optional<std::string> server = config->get_server(key);
  if (!server) return std::nullopt;
  return SimpleClient{*server}.Delete(key);
}

std::optional<std::vector<std::string>> ShardKvClient::MultiGet(
    const std::vector<std::string>& keys) {
  // Query shardmaster for config
  auto config = this->Query();
  if (!config) return std::nullopt;

  // Check that all config servers are valid
  bool valid = true;
  // for each key, find responsible servers in config
  std::map<std::string, std::vector<std::string>> server_keys;
  std::for_each(keys.cbegin(), keys.cend(), [&](auto&& key) {
    std::optional<std::string> server = config->get_server(key);
    if (!server) {
      valid = false;
    } else {
      server_keys[*server].push_back(key);
    }
  });
  // If the config can't find a responsible server for at least one of the keys,
  // return an error
  if (!valid) return std::nullopt;

  // store values associated with each key; use this to maintain order
  std::map<std::string, std::string> key_values;

  // Establish connection to each responsible server, and make MultiGet request
  for (auto&& [server, responsible_keys] : server_keys) {
    auto values = SimpleClient{server}.MultiGet(responsible_keys);
    if (!values) return std::nullopt;
    // if successful, store each key's value in the values list to maintain
    // order
    for (size_t i = 0; i < responsible_keys.size(); i++) {
      key_values[responsible_keys[i]] = (*values)[i];
    }
  }

  // map each key to its corresponding value
  std::vector<std::string> values;
  values.reserve(keys.size());
  for (auto&& key : keys) {
    values.push_back(key_values[key]);
  }
  // return final result!
  return values;
}

bool ShardKvClient::MultiPut(const std::vector<std::string>& keys,
                             const std::vector<std::string>& values) {
  // Query shardmaster for config
  auto config = this->Query();
  if (!config) return false;

  // Check that the config is valid
  bool valid = true;
  // for each key, find responsible servers in config
  // man i love cpp... explore `using std::string`, perhaps?
  std::map<std::string, std::array<std::vector<std::string>, 2>> server_pairs;
  for (size_t i = 0; i < keys.size(); i++) {
    std::optional<std::string> server = config->get_server(keys[i]);
    if (!server) {
      valid = false;
    } else {
      server_pairs[*server][0].push_back(keys[i]);
      server_pairs[*server][1].push_back(values[i]);
    }
  }
  if (!valid) return false;

  // Establish connection to each responsible server, and make MultiPut request
  for (auto&& [server, responsible_pairs] : server_pairs) {
    auto res = SimpleClient{server}.MultiPut(responsible_pairs[0],
                                             responsible_pairs[1]);
    if (!res) return false;
  }

  return true;
}

// Shardmaster functions
std::optional<ShardmasterConfig> ShardKvClient::Query() {
  QueryRequest req;
  if (!this->shardmaster_conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = this->shardmaster_conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* query_res = std::get_if<QueryResponse>(&*res)) {
    return query_res->config;
  }

  return std::nullopt;
}

bool ShardKvClient::Join(const std::string& server) {
  JoinRequest req{server};
  if (!this->shardmaster_conn->send_request(req)) return false;

  std::optional<Response> res = this->shardmaster_conn->recv_response();
  if (!res) return false;
  if (auto* join_res = std::get_if<JoinResponse>(&*res)) {
    return true;
  }

  return false;
}

bool ShardKvClient::Leave(const std::string& server) {
  LeaveRequest req{server};
  if (!this->shardmaster_conn->send_request(req)) return false;

  std::optional<Response> res = this->shardmaster_conn->recv_response();
  if (!res) return false;
  if (auto* leave_res = std::get_if<LeaveResponse>(&*res)) {
    return true;
  }

  return false;
}

bool ShardKvClient::Move(const std::string& server,
                         const std::vector<Shard>& shards) {
  MoveRequest req{server, shards};
  if (!this->shardmaster_conn->send_request(req)) return false;

  std::optional<Response> res = this->shardmaster_conn->recv_response();
  if (!res) return false;
  if (auto* move_res = std::get_if<MoveResponse>(&*res)) {
    return true;
  }

  return false;
}
