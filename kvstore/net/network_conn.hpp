#ifndef NETWORK_CONN_HPP
#define NETWORK_CONN_HPP

#include <netdb.h>
#include <sys/socket.h>

#include <atomic>
#include <memory>
#include <optional>
#include <string>

#include "network_messages.hpp"
#include "server_commands.hpp"
#include "shardmaster_commands.hpp"

/*
 * A wrapper class for a connection with a client, for use by a server
 * (i.e. KvServer, Shardmaster)
 */
struct ClientConn {
  // for make_shared to work
  explicit ClientConn(int fd, std::string addr)
      : fd(fd), address(addr), is_connected(true) {
  }
  ~ClientConn() {
    // cerr_color(YELLOW, "in ClientConn destructor");  // in case if there's a
    // spurious error
    if (this->is_connected) {
      this->is_connected = false;
      ::close(this->fd);
    }
  }

  // The file descriptor for the socket associated with the connection
  int fd;
  // The address (hostname:port) server-client communication occurs over
  std::string address;

  // Whether the client is still connected
  std::atomic<bool> is_connected = true;

  /*
   * Shuts down communication over the socket associated with the connection and
   * destroys it.
   */
  bool close();
  /*
   * Shuts down communication over the socket associated with the connection.
   */
  bool shutdown();

  /*
   * Receives a request from the client, if one has been sent. Otherwise, if the
   * client has disconnected, no request has been sent, or an error occurs, the
   * std::optional returned contains no value.
   */
  std::optional<Request> recv_request();
  /*
   * Sends a given response to the client, returning true on success.
   */
  bool send_response(Response response);
};

/*
 * A wrapper class for a connection with a server, for use by clients
 */
struct ServerConn {
  // for make_shared to work
  explicit ServerConn(int fd, std::string addr) : fd(fd), address(addr) {
  }
  ~ServerConn() {
    // cerr_color(YELLOW, "in ServerConn destructor");  // in case if there's a
    // spurious error
    ::shutdown(this->fd, SHUT_RDWR);
  }

  // The file descriptor for the socket associated with the connection
  int fd;
  // The address (hostname:port) server-client communication occurs over
  std::string address;

  /*
   * Shuts down communication over the socket associated with the connection and
   * destroys it.
   */
  bool close();
  /*
   * Shuts down communication over the socket associated with the connection.
   */
  bool shutdown();

  /*
   * Sends a given request to the server, returning true on success.
   */
  bool send_request(Request request);
  /*
   * Receives a response from the server, if one has been sent. Otherwise, if
   * the server has disconnected, no request has been sent, or an error occurs,
   * the std::optional returned contains no value.
   */
  std::optional<Response> recv_response();
};

/*
 * Waits for and accepts an attempted client connection via the listener socket
 * (specified by its file descriptor). On success, returns a shared pointer to a
 * ClientConn wrapper of the client connection, and a null pointer otherwise.
 */
std::shared_ptr<ClientConn> accept_client(int listener_fd);

/*
 * Establishes a connection to a server at the specified address.
 * On success, returns a shared pointer to a ServerConn wrapper of the server
 * connection, and a null pointer otherwise.
 */
std::shared_ptr<ServerConn> connect_to_server(const std::string& server_addr);

#endif /* end of include guard */
