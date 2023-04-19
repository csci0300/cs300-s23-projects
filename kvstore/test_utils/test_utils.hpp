#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP

#include <algorithm>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "common/shard.hpp"
#include "kvstore/concurrent_kvstore.hpp"
#include "kvstore/simple_kvstore.hpp"
#include "net/network_helpers.hpp"
#include "net/server_commands.hpp"
#include "net/shardmaster_commands.hpp"
#include "server/server.hpp"
#include "shardmaster/shardmaster.hpp"
#include "shardmaster/static_shardmaster.hpp"

using namespace std::chrono;

constexpr int RETRIES = 10;

/**
 * @tparam Type of Server we're creating
 * @tparam Args types of arguments to the Server's constructor
 * @param args args to Server constructor
 *
 * This function creates a server.
 */
template <typename Server, typename... Args>
std::shared_ptr<Server> start_server(Args&&... args) {
  std::shared_ptr<Server> server =
      std::make_shared<Server>(std::forward<Args>(args)...);
  server->start();
  return std::move(server);
}

template <typename Server, typename... Args>
void start_server_in_thread(Args&&... args) {
  std::thread thr(start_server<Server, Args...>, std::forward<Args>(args)...);
  thr.detach();
  // sleep to allow service to start
  std::this_thread::sleep_for(100ms);
}

// TODO: turn Functor and Iterable into a concept
template <typename Functor, typename Iterable>
void execute_in_parallel(Functor func, std::vector<Iterable> chunks) {
  std::vector<std::thread> thrs(chunks.size());
  for (size_t i = 0; i < chunks.size(); i++) {
    thrs[i] = std::thread(func, chunks[i]);
  }
  for (auto&& thr : thrs) {
    thr.join();
  }
}

// ===== Testing asserts

#define ASSERT(test_expr)                                         \
  do {                                                            \
    if (!(test_expr)) {                                           \
      std::cout << __FUNCTION__ << " failed on line " << __LINE__ \
                << std::endl;                                     \
      exit(1);                                                    \
    }                                                             \
  } while (0);

#define ASSERT_EQ(expr1, expr2)                                           \
  do {                                                                    \
    auto lhs = (expr1);                                                   \
    auto rhs = (expr2);                                                   \
    if (lhs != rhs) {                                                     \
      std::cout << __FUNCTION__ << " failed on line " << __LINE__         \
                << ": expected LHS '" << lhs << "' to equal RHS '" << rhs \
                << "'\n";                                                 \
      exit(1);                                                            \
    }                                                                     \
  } while (0);

#define TEST(test_fn, ...)                       \
  do {                                           \
    std::cout << "testing " << #test_fn << "\n"; \
    test_fn(__VA_ARGS__);                        \
  } while (0);

// ===== Miscellaneous utilities

inline std::string random_string(std::size_t str_len) {
  std::string str(
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

  std::random_device rd;
  std::mt19937 generator(rd());

  std::shuffle(str.begin(), str.end(), generator);

  assert(str_len <= str.size());
  return str.substr(0, str_len);
}

inline std::vector<std::string> make_rand_strs(std::size_t num_strs,
                                               std::size_t str_len) {
  std::unordered_set<std::string> strset;
  strset.reserve(num_strs);
  while (strset.size() < num_strs) {
    strset.insert(random_string(str_len));
  }

  std::vector<std::string> strs(strset.begin(), strset.end());
  return strs;
}

inline bool check_equality(std::vector<std::string> v1,
                           std::vector<std::string> v2) {
  std::sort(v1.begin(), v1.end());
  std::sort(v2.begin(), v2.end());

  return v1 == v2;
}

// ===== KvStore testing utilities

inline std::unique_ptr<KvStore> make_kvstore(int argc, char* argv[]) {
  if (argc == 2) {
    auto type = std::string(argv[1]);
    if (type == "simple") {
      return std::make_unique<SimpleKvStore>();
    } else if (type == "concurrent") {
      return std::make_unique<ConcurrentKvStore>();
    } else {
      cerr_color(RED, "Argument must be \"simple\" or \"concurrent\"");
      exit(EXIT_FAILURE);
    }
  } else {
    // Default to ConcurrentKvStore
    return std::make_unique<ConcurrentKvStore>();
  }
}

bool put_range(KvStore& store, const std::vector<std::string>& keys,
               const std::vector<std::string>& vals, std::size_t start,
               std::size_t end);

bool get_range(KvStore& store, const std::vector<std::string>& keys,
               const std::vector<std::string>& vals, std::size_t start,
               std::size_t end);

bool del_range(KvStore& store, const std::vector<std::string>& keys,
               const std::vector<std::string>& vals, std::size_t start,
               std::size_t end);

bool push_get_delete_range(KvStore& store, const std::vector<std::string>& keys,
                           const std::vector<std::string>& vals,
                           std::size_t start, std::size_t end);

bool multiput_range(KvStore& store, const std::vector<std::string>& keys,
                    const std::vector<std::string>& vals, std::size_t start,
                    std::size_t end, std::size_t n_per_mput);

bool multiget_range(KvStore& store, const std::vector<std::string>& keys,
                    const std::vector<std::string>& vals, std::size_t start,
                    std::size_t end, std::size_t n_per_mget);

bool multiput_multiget_range(KvStore& store,
                             const std::vector<std::string>& keys,
                             const std::vector<std::string>& vals,
                             std::size_t start, std::size_t end,
                             std::size_t n_per_mop);

// ===== Shardmaster testing utilities

std::shared_ptr<Shardmaster> start_shardmaster(const std::string& addr);

// Makes a map of server->shards from a vector of servers.
std::map<std::string, std::vector<Shard>> make_shard_map(
    std::vector<std::string> servers);

// Makes n_servers of the form <hostname>:<start_port + i>.
std::vector<std::string> make_servers(size_t n_servers,
                                      uint64_t start_port = 10000);
// Same as above, except chunks the servers into n_chunks. n_servers must be
// divisible by n_chunks.
std::vector<std::vector<std::string>> make_server_chunks(
    size_t n_servers, size_t n_chunks, uint64_t start_port = 10000);

// Joins/leaves the shardmaster on that address.
bool test_join(std::shared_ptr<Shardmaster> sm, const std::string& addr,
               bool success = true);
bool test_leave(std::shared_ptr<Shardmaster> sm, const std::string& addr,
                bool success = true);

// Moves the shards to the address.
bool test_move(std::shared_ptr<Shardmaster> sm, const std::string& addr,
               const std::vector<Shard>& shards, bool success = true);

// Queries the shardmaster. Can either specify a set of shards (irrespective of
// server), or a map of servers to shards.
bool test_query(std::shared_ptr<Shardmaster> sm, std::set<Shard> shards);
bool test_query(std::shared_ptr<Shardmaster> sm,
                std::map<std::string, std::vector<Shard>> m);

// ===== ShardKv testing utilities

// Gets/puts/appends/deletes/multigets/multiputs a key (+ value) on the
// specified KvServer.
bool test_get(const std::string& s_addr, const std::string& key,
              const std::optional<std::string>& value);
bool test_put(const std::string& s_addr, const std::string& key,
              const std::string& value, bool success = true);
bool test_append(const std::string& s_addr, const std::string& key,
                 const std::string& value, bool success = true);
bool test_delete(const std::string& s_addr, const std::string& key,
                 const std::optional<std::string>& value);
bool test_multiget(const std::string& s_addr, const std::string& key,
                   const std::optional<std::vector<std::string>>& value);
bool test_multiput(const std::string& s_addr,
                   const std::vector<std::string>& key,
                   const std::vector<std::string>& value, bool success = true);

#endif /* end of include guard */
