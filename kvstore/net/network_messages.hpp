#ifndef NETWORK_MESSAGES_HPP
#define NETWORK_MESSAGES_HPP

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <chrono>
#include <thread>
#include <variant>
#include <vector>

#include "common/color.hpp"
#include "common/zpp_bits.hpp"
#include "network_helpers.hpp"
#include "server_commands.hpp"
#include "shardmaster_commands.hpp"

// ... cpp chrono is so annoying
using namespace std::chrono;

enum class MessageType {
  // KvServer messages
  GET,
  PUT,
  APPEND,
  DELETE,
  MULTI_GET,
  MULTI_PUT,
  // Shardmaster messages
  JOIN,
  LEAVE,
  MOVE,
  QUERY,
  // Error
  ERROR
};

struct Message {
  MessageType type;
  size_t sz = 0;
  // NOTE: ideally, we wouldn't want memory allocation for every message, but it
  // might be unavoidable due to the variable sizes of strings/vectors :(
  std::vector<std::byte> buf;

  size_t size() {
    return sizeof(type) + sizeof(sz) + buf.size();
  }
};

// Generic send/receive message helper functions.
bool send_message(int fd, Message* msg, milliseconds timeout = 100ms);
bool recv_message(int fd, Message* msg, milliseconds timeout = 100ms);

// define a generic Error response message.
struct ErrorResponse {
  std::string msg;
};

using Request = std::variant<
    // Shardmaster requests
    JoinRequest, LeaveRequest, MoveRequest, QueryRequest,
    // KvServer requests
    GetRequest, PutRequest, AppendRequest, DeleteRequest, MultiGetRequest,
    MultiPutRequest>;
using Response = std::variant<
    // Shardmaster responses
    JoinResponse, LeaveResponse, MoveResponse, QueryResponse,
    // KvServer responses
    GetResponse, PutResponse, AppendResponse, DeleteResponse, MultiGetResponse,
    MultiPutResponse,
    // Error response
    ErrorResponse>;

std::optional<Message> serialize_request(Request request);
std::optional<Request> deserialize_request(Message message);

std::optional<Message> serialize_response(Response response);
std::optional<Response> deserialize_response(Message message);

#endif /* end of include guard */
