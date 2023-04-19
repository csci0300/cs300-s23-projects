#include "static_shardmaster.hpp"

bool StaticShardmaster::Join(const JoinRequest* req, JoinResponse*) {
  // TODO (Part B, Step 2): IMPLEMENT

  return true;
}

bool StaticShardmaster::Leave(const LeaveRequest* req, LeaveResponse*) {
  // TODO (Part B, Step 2): IMPLEMENT

  return true;
}

bool StaticShardmaster::Move(const MoveRequest* req, MoveResponse*) {
  // TODO (Part B, Step 2): IMPLEMENT

  return true;
}

bool StaticShardmaster::Query(const QueryRequest*, QueryResponse* res) {
  // TODO (Part B, Step 2): IMPLEMENT

  return true;
}

/* ==================================================*/
/* === INTERNALS: DO NOT MODIFY BELOW THIS LINE ===  */
/* ==================================================*/

int StaticShardmaster::start() {
  this->is_stopped = false;

  // Create listener socket, and start client listener
  this->listener_fd = open_listener_socket(address);
  if (this->listener_fd < 0) {
    return -1;
  }
  this->client_listener =
      std::thread(&StaticShardmaster::accept_clients_loop, this);

  cout_color(BLUE, "Listening on ", this->address);
  return 0;
}

void StaticShardmaster::stop() {
  this->is_stopped = true;

  // Shutdown listener, and stop accepting clients
  shutdown(this->listener_fd, SHUT_RDWR);
  cout_color(BLUE, "Joining listener thread...");
  this->client_listener.join();

  // Close all connections
  cout_color(BLUE, "Closing all connections...");
  std::unique_lock lock(this->conns_mtx);
  for (auto&& c : this->current_conns) {
    cout_color(BLUE, "Closing connection from ", c->address);
    c->shutdown();
  }
  // Wait for all connections to close (since we've detached threads)
  conns_cv.wait(lock, [this] { return this->current_conns.empty(); });

  // ... and we're done!
}

void StaticShardmaster::accept_clients_loop() {
  while (!this->is_stopped) {
    std::shared_ptr<ClientConn> conn = accept_client(this->listener_fd);
    if (!conn) {
      return;
    }

    // NOTE: for now, let's just spawn a thread to handle each client for
    // simplicity.
    std::unique_lock lock(this->conns_mtx);
    std::thread conn_thread(&StaticShardmaster::handle_client, this, conn);
    conn_thread.detach();
    this->current_conns.push_back(conn);

    cout_color(BLUE, "Received client connection from ", conn->address,
               " on socket ", conn->fd);
  }
}

void StaticShardmaster::handle_client(std::shared_ptr<ClientConn> client) {
  while (!is_stopped && client->is_connected) {
    std::optional<Request> req = client->recv_request();
    if (!req) {
      break;
    }

    Response res = this->process_request(*req);
    if (!client->send_response(res)) {
      break;
    }
  }
  // Regardless of whether the shardmaster is stopped or a message failed to be
  // sent/received, clean up client (should be automatically freed once it goes
  // out of scope)

  client->shutdown();

  // remove from existing connections
  std::unique_lock lock(this->conns_mtx);
  if (auto it = std::find(this->current_conns.begin(),
                          this->current_conns.end(), client);
      it != this->current_conns.end()) {
    this->current_conns.erase(it);
  } else {
    cerr_color(YELLOW,
               "Connection should not already be removed; please post "
               "privately on Edstem if "
               "you receieve this error!");
  }

  // notify waiting cv if last connection
  if (this->current_conns.empty()) {
    this->conns_cv.notify_all();
  }
}

Response StaticShardmaster::process_request(Request req) {
  Response res;
  if (auto* join_req = std::get_if<JoinRequest>(&req)) {
    JoinResponse join_res{};
    if (this->Join(join_req, &join_res)) {
      res = join_res;
    } else {
      res = ErrorResponse{"Failed to process Join request."};
    }
  } else if (auto* leave_req = std::get_if<LeaveRequest>(&req)) {
    LeaveResponse leave_res{};
    if (this->Leave(leave_req, &leave_res)) {
      res = leave_res;
    } else {
      res = ErrorResponse{"Failed to process Leave request."};
    }
  } else if (auto* move_req = std::get_if<MoveRequest>(&req)) {
    MoveResponse move_res{};
    if (this->Move(move_req, &move_res)) {
      res = move_res;
    } else {
      res = ErrorResponse{"Failed to process Move request."};
    }
  } else if (auto* query_req = std::get_if<QueryRequest>(&req)) {
    QueryResponse query_res{};
    if (this->Query(query_req, &query_res)) {
      res = query_res;
    } else {
      res = ErrorResponse{"Failed to process Query request."};
    }
  } else {
    throw std::logic_error{"invalid request variant!"};
  }
  return res;
}
