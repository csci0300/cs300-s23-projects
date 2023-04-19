#include "server.hpp"

int KvServer::start() {
  this->is_stopped = false;

  // Initialize KvStore
  // TODO (Part A, Step 4): Change your underlying KvStore to the
  // ConcurrentKvStore!
  this->store = std::make_unique<SimpleKvStore>();

  // Create listener socket, and start client listener
  this->listener_fd = open_listener_socket(address);
  if (this->listener_fd < 0) {
    return -1;
  }
  this->client_listener = std::thread(&KvServer::accept_clients_loop, this);
  cout_color(BLUE, "Listening on: ", this->address);

  // Initialize worker threads
  this->workers.resize(this->n_workers);
  for (auto&& worker : this->workers) {
    worker = std::thread(&KvServer::work_loop, this);
  }

  // If shardmaster address not empty, connect to shardmaster and start query
  // thread
  if (!this->shardmaster_address.empty()) {
    this->shardmaster_conn = connect_to_server(this->shardmaster_address);
    if (!this->shardmaster_conn) {
      close(this->listener_fd);
      return -1;
    }

    this->shardmaster_querier =
        std::thread(&KvServer::query_shardmaster_loop, this);
    cout_color(BLUE, "Shardmaster on: ", this->shardmaster_address);
  }

  return 0;
}

void KvServer::stop() {
  this->is_stopped = true;

  // Close client listener
  shutdown(this->listener_fd, SHUT_RDWR);
  cout_color(BLUE, "Joining client listener thread...");
  this->client_listener.join();

  // Stop connection queue, and close & join workers
  this->conn_queue.stop();
  for (auto&& client : this->conn_queue.flush()) {
    cout_color(BLUE, "Closing connection from ", client->address);
    client->shutdown();
  }
  for (auto&& thr : this->workers) thr.join();

  // If shardmaster exists, join shardmaster querier thread, and close
  // shardmaster connection
  if (!this->shardmaster_address.empty()) {
    cout_color(BLUE, "Joining query shardmaster thread...");
    this->shardmaster_querier.join();
    this->shardmaster_conn->shutdown();
  }
}

std::map<std::string, std::string> KvServer::all_kvpairs() {
  auto keys = this->store->AllKeys();
  std::map<std::string, std::string> map;
  for (auto&& k : keys) {
    auto req = GetRequest{k};
    auto res = GetResponse{};
    // Only add if key still exists
    if (this->store->Get(&req, &res)) {
      map[k] = res.value;
    }
  }

  return map;
}

ShardmasterConfig KvServer::get_config() {
  return this->config;
}

void KvServer::accept_clients_loop() {
  // While the server is not stopped, accept clients from the listener socket,
  // then add them to the work queue.
  while (!this->is_stopped.load()) {
    std::shared_ptr<ClientConn> client = accept_client(this->listener_fd);
    if (!client) {
      return;
    }
    cout_color(BLUE, "Received client connection from ", client->address,
               " on socket ", client->fd);

    this->conn_queue.push(client);
  }
}

void KvServer::work_loop() {
  // Each worker thread will run this function. While the server is not stopped,
  // pop an accepted connection off of the work queue, and process client
  // requests until the client closes the connection.
  while (!this->is_stopped) {
    std::shared_ptr<ClientConn> client;
    // if this returns false, queue stopped
    if (bool stopped = this->conn_queue.pop(&client); stopped) {
      break;
    }

    while (true) {
      std::optional<Request> req = client->recv_request();
      if (!req) {
        client->close();
        break;
      }
      Response res = this->process_request(*req);
      if (auto* error_res = std::get_if<ErrorResponse>(&res)) {
        cerr_color(RED, "Request failed: ", error_res->msg);
      }
      if (!client->send_response(res)) {
        client->close();
        break;
      }
    }
  }
}

bool KvServer::responsible_for(const std::string& key) {
  // For Concurrent Store, no shardmaster exists, so no-op
  if (this->shardmaster_address.empty()) return true;

  // TODO(Part B, Step 2): IMPLEMENT

  return true;
}

bool KvServer::responsible_for(const std::vector<std::string>& keys) {
  // For Concurrent Store, no shardmaster exists, so no-op
  if (this->shardmaster_address.empty()) return true;

  // TODO(Part B, Step 2): IMPLEMENT

  return true;
}

bool KvServer::query_shardmaster() {
  // TODO(Part B, Step 2): IMPLEMENT

  return true;
}

/* ==================================================*/
/* === INTERNALS: DO NOT MODIFY BELOW THIS LINE ===  */
/* ==================================================*/

Response KvServer::process_request(Request req) {
  Response res;
  if (auto* get_req = std::get_if<GetRequest>(&req)) {
    bool responsible = this->responsible_for(get_req->key);
    GetResponse get_res;
    if (responsible && this->store->Get(get_req, &get_res)) {
      res = get_res;
    } else {
      res = ErrorResponse{
          !responsible ? std::string("server not responsible for key")
                       : std::string("key does not exist in the KVStore")};
    }
  } else if (auto* put_req = std::get_if<PutRequest>(&req)) {
    bool responsible = this->responsible_for(put_req->key);
    PutResponse put_res;
    if (responsible && this->store->Put(put_req, &put_res)) {
      res = put_res;
    } else {
      // Put should never fail
      res = ErrorResponse{!responsible
                              ? std::string("server not responsible for key")
                              : std::string("internal KVStore error")};
    }
  } else if (auto* append_req = std::get_if<AppendRequest>(&req)) {
    bool responsible = this->responsible_for(append_req->key);
    AppendResponse append_res;
    if (responsible && this->store->Append(append_req, &append_res)) {
      res = append_res;
    } else {
      res = ErrorResponse{!responsible
                              ? std::string("server not responsible for key")
                              : std::string("internal KVStore error")};
    }
  } else if (auto* delete_req = std::get_if<DeleteRequest>(&req)) {
    bool responsible = this->responsible_for(delete_req->key);
    DeleteResponse delete_res;
    if (responsible && this->store->Delete(delete_req, &delete_res)) {
      res = delete_res;
    } else {
      res = ErrorResponse{
          !responsible ? std::string("server not responsible for key")
                       : std::string("key does not exist in the KVStore")};
    }
  } else if (auto* multiget_req = std::get_if<MultiGetRequest>(&req)) {
    bool responsible = this->responsible_for(multiget_req->keys);
    MultiGetResponse multiget_res;
    if (responsible && this->store->MultiGet(multiget_req, &multiget_res)) {
      res = multiget_res;
    } else {
      res = ErrorResponse{
          !responsible ? std::string("server not responsible for key(s)")
                       : std::string("key(s) do not exist in the KVStore")};
    }
  } else if (auto* multiput_req = std::get_if<MultiPutRequest>(&req)) {
    bool responsible = this->responsible_for(multiput_req->keys);
    MultiPutResponse multiput_res;
    if (responsible && this->store->MultiPut(multiput_req, &multiput_res)) {
      res = multiput_res;
    } else {
      res = ErrorResponse{!responsible
                              ? std::string("server not responsible for key(s)")
                              : std::string("internal KVStore error")};
    }
  } else {
    throw std::logic_error{"invalid variant!"};
  }
  return res;
}

void KvServer::query_shardmaster_loop() {
  while (!this->is_stopped) {
    if (!this->query_shardmaster()) {
      // NOTE: we really should add more logic here, e.g. tolerate a few
      // failures
      break;
    }
    std::this_thread::sleep_for(250ms);
  }
}

std::optional<QueryResponse> KvServer::get_query_response(
    std::shared_ptr<ServerConn> conn) {
  auto res = conn->recv_response();
  if (!res) {
    return std::nullopt;
  }
  auto* query_res = std::get_if<QueryResponse>(&*res);
  if (!query_res) {
    return std::nullopt;
  }
  return *query_res;
}
