#include "simple_client.hpp"

std::optional<std::string> SimpleClient::Get(const std::string& key) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  GetRequest req{key};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* get_res = std::get_if<GetResponse>(&*res)) {
    return get_res->value;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Get value from server: ", error_res->msg);
  }

  return std::nullopt;
}

bool SimpleClient::Put(const std::string& key, const std::string& value) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  PutRequest req{key, value};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* put_res = std::get_if<PutResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Put value to server: ", error_res->msg);
  }

  return false;
}

bool SimpleClient::Append(const std::string& key, const std::string& value) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  AppendRequest req{key, value};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* append_res = std::get_if<AppendResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Append value to server: ", error_res->msg);
  }

  return false;
}

std::optional<std::string> SimpleClient::Delete(const std::string& key) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  DeleteRequest req{key};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* delete_res = std::get_if<DeleteResponse>(&*res)) {
    return delete_res->value;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to Delete value on server: ", error_res->msg);
  }

  return std::nullopt;
}

std::optional<std::vector<std::string>> SimpleClient::MultiGet(
    const std::vector<std::string>& keys) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return std::nullopt;
  }

  MultiGetRequest req{keys};
  if (!conn->send_request(req)) return std::nullopt;

  std::optional<Response> res = conn->recv_response();
  if (!res) return std::nullopt;
  if (auto* multiget_res = std::get_if<MultiGetResponse>(&*res)) {
    return multiget_res->values;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to MultiGet values on server: ", error_res->msg);
  }

  return std::nullopt;
}

bool SimpleClient::MultiPut(const std::vector<std::string>& keys,
                            const std::vector<std::string>& values) {
  std::shared_ptr<ServerConn> conn = connect_to_server(this->server_addr);
  if (!conn) {
    cerr_color(RED, "Failed to connect to KvServer at ", this->server_addr,
               '.');
    return false;
  }

  MultiPutRequest req{keys, values};
  if (!conn->send_request(req)) return false;

  std::optional<Response> res = conn->recv_response();
  if (!res) return false;
  if (auto* multiput_res = std::get_if<MultiPutResponse>(&*res)) {
    return true;
  } else if (auto* error_res = std::get_if<ErrorResponse>(&*res)) {
    cerr_color(RED, "Failed to MultiPut values on server: ", error_res->msg);
  }

  return false;
}

bool SimpleClient::GDPRDelete(const std::string& user) {
  // TODO: Write your GDPR deletion code here!
  // You can invoke operations directly on the client object, like so:
  //
  // std::string key("user_1_posts");
  // std::optional<std::string> posts = Get(key);
  // ...
  //
  // Assume the `user` arugment is a user ID such as "user_1".

  cerr_color(RED, "GDPR deletion is unimplemented!");
  return false;
}
