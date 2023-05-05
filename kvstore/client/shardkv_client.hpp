#ifndef SHARDKV_CLIENT_HPP
#define SHARDKV_CLIENT_HPP

#include <array>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "client.hpp"
#include "common/config.hpp"
#include "net/network_conn.hpp"
#include "net/network_messages.hpp"
#include "simple_client.hpp"

class ShardKvClient : public Client {
 public:
  explicit ShardKvClient(const std::string& sm_addr)
      : shardmaster_addr(sm_addr) {
    this->shardmaster_conn = connect_to_server(this->shardmaster_addr);
    if (!this->shardmaster_conn) {
      cerr_color(RED, "Failed to connect to shardmaster at ",
                 this->shardmaster_addr, '.');
      exit(EXIT_FAILURE);
    }
    cout_color(BLUE, "Connected to shardmaster at ", this->shardmaster_addr,
               '.');
  }

  ~ShardKvClient() {
    this->shardmaster_conn->shutdown();
  }

  // ShardKvStore functions
  std::optional<std::string> Get(const std::string& key);

  bool Put(const std::string& key, const std::string& value);

  bool Append(const std::string& key, const std::string& value);

  std::optional<std::string> Delete(const std::string& key);

  std::optional<std::vector<std::string>> MultiGet(
      const std::vector<std::string>& keys);

  bool MultiPut(const std::vector<std::string>& keys,
                const std::vector<std::string>& values);
  
  bool GDPRDelete(const std::string& user) {
    assert(false);
  }

  // Shardmaster functions
  std::optional<ShardmasterConfig> Query();

  bool Join(const std::string& server);

  bool Leave(const std::string& server);

  bool Move(const std::string& server, const std::vector<Shard>& shards);

 private:
  std::string shardmaster_addr;
  std::shared_ptr<ServerConn> shardmaster_conn;
};

#endif /* end of include guard */
