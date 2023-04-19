#include "network_conn.hpp"

bool ClientConn::close() {
  if (this->is_connected) {
    this->is_connected = false;
    ::close(this->fd);
  }
  return true;
}

bool ClientConn::shutdown() {
  if (this->is_connected) {
    this->is_connected = false;
    ::shutdown(this->fd, SHUT_RDWR);
  }
  return true;
}

std::optional<Request> ClientConn::recv_request() {
  Message msg{};
  if (!recv_message(fd, &msg)) {
    return std::nullopt;
  }

  auto req = deserialize_request(msg);
  if (!req) {
    perror_color(RED, "Error deserializing request.");
  }
  return req;
}

bool ClientConn::send_response(Response response) {
  std::optional<Message> msg = serialize_response(response);
  if (!msg) {
    perror_color(RED, "Error serializing response.");
    return false;
  }

  return send_message(fd, &*msg);
}

bool ServerConn::close() {
  ::close(this->fd);
  return true;
}

bool ServerConn::shutdown() {
  ::shutdown(this->fd, SHUT_RDWR);
  return true;
}

bool ServerConn::send_request(Request req) {
  std::optional<Message> msg = serialize_request(req);
  if (!msg) {
    perror_color(RED, "Error serializing request.");
    return false;
  }

  return send_message(fd, &*msg);
}

std::optional<Response> ServerConn::recv_response() {
  Message msg{};
  if (!recv_message(fd, &msg)) {
    return std::nullopt;
  }

  auto res = deserialize_response(msg);
  if (!res) {
    perror_color(RED, "Error deserializing response.");
  }
  return res;
}

std::shared_ptr<ClientConn> accept_client(int listener_fd) {
  // NOTE: ideally, we should use a sockaddr_storage and handle INET vs INET6,
  // but since we're only supporting IPv4 here, this should be fine.
  struct sockaddr_in client_addr;
  socklen_t sin_size = sizeof(client_addr);
  int cfd = accept(listener_fd, (struct sockaddr*)&client_addr, &sin_size);
  if (cfd < 0) {
    // perror_color(RED, "accept");
    return nullptr;
  }

  // get hostname:port for presentability.
  char hostbuf[NI_MAXHOST], servbuf[NI_MAXSERV];
  if (getnameinfo((struct sockaddr*)&client_addr, sin_size, hostbuf,
                  sizeof(hostbuf), servbuf, sizeof(servbuf),
                  NI_NUMERICSERV) != 0) {
    perror_color(RED, "getnameinfo");
    return nullptr;
  }
  char s[NI_MAXHOST + NI_MAXSERV] = {0};
  snprintf(s, sizeof(s), "%s:%s", hostbuf, servbuf);

  return std::make_shared<ClientConn>(cfd, std::string(s));
}

std::shared_ptr<ServerConn> connect_to_server(const std::string& server_addr) {
  int sfd = connect_to_address(server_addr);
  if (sfd < 0) {
    return nullptr;
  }

  return std::make_shared<ServerConn>(sfd, server_addr);
}
