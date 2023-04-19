#include "network_messages.hpp"

bool send_message(int fd, Message* msg, milliseconds timeout) {
  // must specify non-zero timeout
  assert(timeout > 0ms);
  assert(msg->sz == msg->buf.size());

  // First, send message type
  int curr = sendall(fd, &msg->type, sizeof(msg->type), MSG_NOSIGNAL);
  if (curr < 0) {
    if (curr == ETIMEOUT) {
      // Print if timed out
      cerr_color(RED, "Send on ", fd, " timed out.");
    } else if (errno != EBADF && errno != EPIPE) {
      // Only emit errors if it wasn't the result of the socket closing
      perror_color(RED, "send");
    }
    return false;
  }
  assert(curr == sizeof(msg->type));

  // Convert size to network order, then send
  size_t size_nbo = htonl(msg->sz);
  curr = sendall(fd, &size_nbo, sizeof(size_nbo), MSG_NOSIGNAL);
  if (curr < 0) {
    if (curr == ETIMEOUT) {
      cerr_color(RED, "Send on ", fd, " timed out.");
    } else if (errno != EBADF && errno != EPIPE) {
      perror_color(RED, "send");
    }
    return false;
  }
  assert(curr == sizeof(size_nbo));

  if (msg->sz > 0) {
    std::byte* data = &msg->buf[0];
    curr = sendall(fd, data, msg->sz, 0, timeout);
    if (curr < 0) {
      if (curr == ETIMEOUT) {
        cerr_color(RED, "Send on ", fd, " timed out.");
      } else if (errno != EBADF && errno != EPIPE) {
        perror_color(RED, "send");
      }
      return false;
    }
    assert(size_t(curr) == msg->sz);
  }

  return true;
}

bool recv_message(int fd, Message* msg, milliseconds timeout) {
  // NOTE: Re-visit this later.
  //
  // I think it'd be okay if we re-use the structure from send, but it would
  // depend on our client implementation of message sending: if the client
  // creates a new connection, makes the request, then closes the connection,
  // this would be okay. However, if the connection persists, we would want to
  // block until we receive a response, rather than "failing" the recv_message
  // call.

  // must specify non-zero timeout
  assert(timeout > 0ms);

  // get message type
  int curr = recvall(fd, &msg->type, sizeof(msg->type), 0);
  if (curr == 0) {
    // In this case, recv got an EOF, so other end closed the connection.
    return false;
  } else if (curr < 0) {
    if (curr == ETIMEOUT) {
      // Print if timed out
      cerr_color(RED, "Recv on ", fd, " timed out.");
    } else if (errno != EBADF) {
      // Only emit errors if it wasn't the result of the socket closing
      perror_color(RED, "recv");
    }
    return false;
  }
  assert(curr == sizeof(msg->type));

  // get size; need this to inform how much to read into the vector
  curr = recvall(fd, &msg->sz, sizeof(msg->sz), 0);
  if (curr == 0) {
    return false;
  } else if (curr < 0) {
    if (curr == ETIMEOUT) {
      cerr_color(RED, "Recv on ", fd, " timed out.");
    } else if (errno != EBADF) {
      perror_color(RED, "recv");
    }
    return false;
  }
  // Convert to host order
  msg->sz = ntohl(msg->sz);
  assert(curr == sizeof(msg->sz));

  if (msg->sz > 0) {
    msg->buf.resize(msg->sz);
    std::byte* data = &msg->buf[0];
    // Later, let's explore MSG_DONTWAIT (need to check if errno != EAGAIN or
    // EWOULDBLOCK)
    curr = recvall(fd, data, msg->sz, 0, timeout);
    if (curr == 0) {
      return false;
    } else if (curr < 0) {
      if (curr == ETIMEOUT) {
        cerr_color(RED, "Recv on ", fd, " timed out.");
      } else if (errno != EBADF) {
        perror_color(RED, "recv");
      }
      return false;
    }
    assert(size_t(curr) == msg->sz);
  }

  return true;
}

std::optional<Message> serialize_request(Request request) {
  Message msg{};

  // Serialize into message, depending on type
  auto out = zpp::bits::output(msg.buf);
  if (auto* req = std::get_if<JoinRequest>(&request)) {
    msg.type = MessageType::JOIN;
    if (!success(out(*req))) return std::nullopt;
  } else if (auto* req = std::get_if<LeaveRequest>(&request)) {
    msg.type = MessageType::LEAVE;
    if (!success(out(*req))) return std::nullopt;
  } else if (auto* req = std::get_if<MoveRequest>(&request)) {
    msg.type = MessageType::MOVE;
    if (!success(out(*req))) return std::nullopt;
  } else if (auto* req = std::get_if<QueryRequest>(&request)) {
    msg.type = MessageType::QUERY;
    if (!success(out(*req))) return std::nullopt;
  } else if (auto* req = std::get_if<GetRequest>(&request)) {
    msg.type = MessageType::GET;
    if (!success(out(*req))) return std::nullopt;
  } else if (auto* req = std::get_if<PutRequest>(&request)) {
    msg.type = MessageType::PUT;
    if (!success(out(*req))) return std::nullopt;
  } else if (auto* req = std::get_if<AppendRequest>(&request)) {
    msg.type = MessageType::APPEND;
    if (!success(out(*req))) return std::nullopt;
  } else if (auto* req = std::get_if<DeleteRequest>(&request)) {
    msg.type = MessageType::DELETE;
    if (!success(out(*req))) return std::nullopt;
  } else if (auto* req = std::get_if<MultiGetRequest>(&request)) {
    msg.type = MessageType::MULTI_GET;
    if (!success(out(*req))) return std::nullopt;
  } else if (auto* req = std::get_if<MultiPutRequest>(&request)) {
    msg.type = MessageType::MULTI_PUT;
    if (!success(out(*req))) return std::nullopt;
  } else {
    throw std::logic_error{
        "Invalid request variant! Please post privately on Edstem if this "
        "occurs."};
  }

  // Set size, for easier network parsing
  msg.sz = msg.buf.size();

  return msg;
}

std::optional<Request> deserialize_request(Message message) {
  Request request;
  // Deserialize from message, depending on type
  auto in = zpp::bits::input(message.buf);
  switch (message.type) {
    case MessageType::JOIN: {
      JoinRequest req{};
      if (!success(in(req))) return std::nullopt;
      request = req;
      break;
    }
    case MessageType::LEAVE: {
      LeaveRequest req{};
      if (!success(in(req))) return std::nullopt;
      request = req;
      break;
    }
    case MessageType::MOVE: {
      MoveRequest req{};
      if (!success(in(req))) return std::nullopt;
      request = req;
      break;
    }
    case MessageType::QUERY: {
      QueryRequest req{};
      if (!success(in(req))) return std::nullopt;
      request = req;
      break;
    }
    case MessageType::GET: {
      GetRequest req{};
      if (!success(in(req))) return std::nullopt;
      request = req;
      break;
    }
    case MessageType::PUT: {
      PutRequest req{};
      if (!success(in(req))) return std::nullopt;
      request = req;
      break;
    }
    case MessageType::APPEND: {
      AppendRequest req{};
      if (!success(in(req))) return std::nullopt;
      request = req;
      break;
    }
    case MessageType::DELETE: {
      DeleteRequest req{};
      if (!success(in(req))) return std::nullopt;
      request = req;
      break;
    }
    case MessageType::MULTI_GET: {
      MultiGetRequest req{};
      if (!success(in(req))) return std::nullopt;
      request = req;
      break;
    }
    case MessageType::MULTI_PUT: {
      MultiPutRequest req{};
      if (!success(in(req))) return std::nullopt;
      request = req;
      break;
    }
    default:
      throw std::logic_error{
          "Invalid message type! Please post privately on Edstem if this "
          "occurs."};
      break;
  };

  return request;
}

std::optional<Message> serialize_response(Response response) {
  Message msg{};

  // Serialize into message, depending on type
  auto out = zpp::bits::output(msg.buf);
  if (auto* res = std::get_if<JoinResponse>(&response)) {
    msg.type = MessageType::JOIN;
    if (!success(out(*res))) return std::nullopt;
  } else if (auto* res = std::get_if<LeaveResponse>(&response)) {
    msg.type = MessageType::LEAVE;
    if (!success(out(*res))) return std::nullopt;
  } else if (auto* res = std::get_if<MoveResponse>(&response)) {
    msg.type = MessageType::MOVE;
    if (!success(out(*res))) return std::nullopt;
  } else if (auto* res = std::get_if<QueryResponse>(&response)) {
    msg.type = MessageType::QUERY;
    if (!success(out(*res))) return std::nullopt;
  } else if (auto* res = std::get_if<GetResponse>(&response)) {
    msg.type = MessageType::GET;
    if (!success(out(*res))) return std::nullopt;
  } else if (auto* res = std::get_if<PutResponse>(&response)) {
    msg.type = MessageType::PUT;
    if (!success(out(*res))) return std::nullopt;
  } else if (auto* res = std::get_if<AppendResponse>(&response)) {
    msg.type = MessageType::APPEND;
    if (!success(out(*res))) return std::nullopt;
  } else if (auto* res = std::get_if<DeleteResponse>(&response)) {
    msg.type = MessageType::DELETE;
    if (!success(out(*res))) return std::nullopt;
  } else if (auto* res = std::get_if<MultiGetResponse>(&response)) {
    msg.type = MessageType::MULTI_GET;
    if (!success(out(*res))) return std::nullopt;
  } else if (auto* res = std::get_if<MultiPutResponse>(&response)) {
    msg.type = MessageType::MULTI_PUT;
    if (!success(out(*res))) return std::nullopt;
  } else if (auto* res = std::get_if<ErrorResponse>(&response)) {
    msg.type = MessageType::ERROR;
    if (!success(out(*res))) return std::nullopt;
  } else {
    throw std::logic_error{
        "Invalid response variant! Please post privately on Edstem if this "
        "occurs."};
  }

  // Set size, for easier network parsing
  msg.sz = msg.buf.size();

  return msg;
}

std::optional<Response> deserialize_response(Message message) {
  Response response;
  // Deserialize from message, depending on type
  auto in = zpp::bits::input(message.buf);
  switch (message.type) {
    case MessageType::JOIN: {
      JoinResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    case MessageType::LEAVE: {
      LeaveResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    case MessageType::MOVE: {
      MoveResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    case MessageType::QUERY: {
      QueryResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    case MessageType::GET: {
      GetResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    case MessageType::PUT: {
      PutResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    case MessageType::APPEND: {
      AppendResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    case MessageType::DELETE: {
      DeleteResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    case MessageType::MULTI_GET: {
      MultiGetResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    case MessageType::MULTI_PUT: {
      MultiPutResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    case MessageType::ERROR: {
      ErrorResponse res{};
      if (!success(in(res))) return std::nullopt;
      response = res;
      break;
    }
    default:
      throw std::logic_error{
          "Invalid message type! Please post privately on Edstem if this "
          "occurs."};
      break;
  };

  return response;
}
