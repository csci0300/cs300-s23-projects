#ifndef SERVER_COMMANDS_HPP
#define SERVER_COMMANDS_HPP

#include <string>
#include <variant>
#include <vector>

// Requests
struct GetRequest {
  std::string key;
};

struct PutRequest {
  std::string key;
  std::string value;
};

struct AppendRequest {
  std::string key;
  std::string value;
};

struct DeleteRequest {
  std::string key;
};

struct MultiGetRequest {
  std::vector<std::string> keys;
};

struct MultiPutRequest {
  std::vector<std::string> keys;
  std::vector<std::string> values;
};

// Responses
struct GetResponse {
  std::string value;
};

struct PutResponse {};
struct AppendResponse {};
struct DeleteResponse {
  std::string value;
};
struct MultiGetResponse {
  std::vector<std::string> values;
};
struct MultiPutResponse {};

#endif /* end of include guard */
