#ifndef STATIC_SHARDMASTER_HPP
#define STATIC_SHARDMASTER_HPP

#include <condition_variable>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <vector>

#include "common/config.hpp"
#include "common/shard.hpp"
#include "net/network_conn.hpp"
#include "net/network_helpers.hpp"
#include "net/network_messages.hpp"
#include "shardmaster.hpp"

class StaticShardmaster : public Shardmaster {
 public:
  explicit StaticShardmaster(const std::string& addr) : address(addr) {
  }
  ~StaticShardmaster() {
    if (!this->is_stopped) {
      this->stop();
    }
  }

  bool Join(const JoinRequest* req, JoinResponse*) override;
  bool Leave(const LeaveRequest* req, LeaveResponse*) override;
  bool Move(const MoveRequest* req, MoveResponse*) override;
  bool Query(const QueryRequest*, QueryResponse* res) override;

  int start() override;
  void stop() override;

 private:
  // TODO: store the current shard configuration. You will need to add fields!

  /* ==================================================*/
  /* === INTERNALS: DO NOT MODIFY BELOW THIS LINE ===  */
  /* ==================================================*/
  /* You might find it helpful to review these fields before implementing
   * Part 2. */

  // An atomic, thread-safe boolean to denote whether the shardmaster has been
  // stopped.
  std::atomic<bool> is_stopped;

  // Address on which the shardmaster is listening.
  std::string address;

  // Listener socket for incoming client connections.
  int listener_fd;
  // Thread that listens for client connections and accepts them.
  std::thread client_listener;

  // Collection of current client connections.
  std::vector<std::shared_ptr<ClientConn>> current_conns;
  // Synchronization primitives for client connections.
  std::mutex conns_mtx;
  std::condition_variable conns_cv;

  /**
   * In a loop, accept client connections, then pass each connection into the
   * work queue of client connections to process.
   *
   * Exits when the server has been stopped.
   */
  void accept_clients_loop();

  /**
   * Handle the associated client for the duration that it's connected.
   */
  void handle_client(std::shared_ptr<ClientConn> client);

  /**
   * Process an incoming request: parse its request type, call its appropriate
   * handler (Join, Leave, ...), then get a response.
   */
  Response process_request(Request req);
};

#endif /* end of include guard */
