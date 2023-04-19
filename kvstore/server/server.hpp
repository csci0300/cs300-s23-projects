#ifndef KVSERVER_HPP
#define KVSERVER_HPP

#include <array>
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <utility>

#include "kvstore/concurrent_kvstore.hpp"
#include "kvstore/kvstore.hpp"
#include "kvstore/simple_kvstore.hpp"
#include "net/network_conn.hpp"
#include "net/network_helpers.hpp"
#include "net/network_messages.hpp"
#include "synchronized_queue.hpp"

#define N_WORKERS 5

using namespace std::chrono;

class KvServer {
 public:
  explicit KvServer(const std::string& address, uint64_t n_workers)
      : address(address), shardmaster_address(), n_workers(n_workers) {
  }
  explicit KvServer(const std::string& address,
                    const std::string& shardmaster_addr, uint64_t n_workers)
      : address(address),
        shardmaster_address(shardmaster_addr),
        n_workers(n_workers) {
  }
  ~KvServer() {
    if (!this->is_stopped) {
      this->stop();
    }
  }

  // Starts the KvServer.
  int start();
  void stop();

  // For debugging purposes, get all key-value pairs from the store. Note that
  // this operation is not atomic; do not attempt to use this to atomically
  // retrieve key-value pairs!
  std::map<std::string, std::string> all_kvpairs();

  // For debugging purposes, get the shardmaster config from the server.
  ShardmasterConfig get_config();

  KvServer(const KvServer&) = delete;
  KvServer& operator=(const KvServer&) = delete;

 private:
  /* ========================================================================*/
  /* === NOTE: You will need these fields for Part A: Concurrent Store! ===  */
  /* ========================================================================*/
  /* You might find it helpful to review these fields before implementing Part
   * A, Step 2.
   *
   * You do not (and should not) need to touch any of these fields.
   */

  // An atomic, thread-safe boolean to denote whether the server has been
  // stopped.
  std::atomic<bool> is_stopped;

  // The address on which the server/shardmaster is listening.
  std::string address;
  std::string shardmaster_address;

  // Listener socket for incoming client connections.
  int listener_fd;
  // Thread that listens for client connections and accepts them.
  std::thread client_listener;

  // Vector of worker threads.
  std::vector<std::thread> workers;
  // Number of worker threads.
  uint64_t n_workers;

  // Thread-safe work queue of current client connections.
  synchronized_queue<std::shared_ptr<ClientConn>> conn_queue;

  // Internal key-value store.
  std::unique_ptr<KvStore> store;

  /**
   * In a loop, accept client connections, then pass each connection into the
   * work queue of client connections to process.
   *
   * Exits when the server has been stopped.
   */
  void accept_clients_loop();

  /**
   * In a loop, pop a client connection from the work queue and process a
   * request from it.
   *
   * Exits when the server has been stopped.
   */
  void work_loop();

  /* =========================================================================*/
  /* === NOTE: You will need these fields for Part B: Distributed Store! ===  */
  /* =========================================================================*/
  /* You might find it helpful to review these fields before implementing Part
   * B, Step 2.
   *
   * For this part, you might need to modify or add fields. :)
   */

  // Persistent shardmaster connection.
  std::shared_ptr<ServerConn> shardmaster_conn;
  // Thread that periodically queries the shardmaster for the current
  // configuration.
  std::thread shardmaster_querier;  // bro this name goofy
  // Shardmaster configuration.
  ShardmasterConfig config;

  // TODO (Part B, Step 2): Add any fields that you will need to synchronize
  // access to the ShardmasterConfig!

  /**
   * Check whether this server is responsible for a key (or list of keys).
   */
  bool responsible_for(const std::string& key);
  bool responsible_for(const std::vector<std::string>& keys);

  /**
   * Query the shardmaster, then update the config with new values and move
   * outdated pairs to updated servers.
   */
  bool query_shardmaster();

  /* ==================================================*/
  /* === INTERNALS: DO NOT MODIFY BELOW THIS LINE ===  */
  /* ==================================================*/

  /**
   * Process an incoming request: parse its request type, call its appropriate
   * handler (Get, Put, etc.), then get a response.
   */
  Response process_request(Request req);

  // Extracts a query response from a connection, or an std::nullopt if one
  // doesn't exist. You might need this when implementing query_shardmaster!
  std::optional<QueryResponse> get_query_response(
      std::shared_ptr<ServerConn> conn);

  // Wrapper function that calls query_shardmaster periodically.
  void query_shardmaster_loop();
};

#endif /* end of include guard */
