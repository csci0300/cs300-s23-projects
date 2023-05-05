#ifndef SIMPLE_CLIENT_HPP
#define SIMPLE_CLIENT_HPP

#include <optional>
#include <string>

#include "client.hpp"
#include "net/network_conn.hpp"

class SimpleClient : public Client {
 public:
  explicit SimpleClient(const std::string& server_addr)
      : server_addr(server_addr) {
  }
  ~SimpleClient() = default;

  // ShardKvStore functions.
  std::optional<std::string> Get(const std::string& key);

  bool Put(const std::string& key, const std::string& value);

  bool Append(const std::string& key, const std::string& value);

  std::optional<std::string> Delete(const std::string& key);

  std::optional<std::vector<std::string>> MultiGet(
      const std::vector<std::string>& keys);

  bool MultiPut(const std::vector<std::string>& keys,
                const std::vector<std::string>& values);

  bool GDPRDelete(const std::string& user);

 private:
  std::string server_addr;
};

#endif /* end of include guard */
