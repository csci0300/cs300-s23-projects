#ifndef SHARDMASTER_COMMANDS_HPP
#define SHARDMASTER_COMMANDS_HPP

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "common/config.hpp"
#include "network_helpers.hpp"

// Requests
struct JoinRequest {
  std::string server;
};
struct LeaveRequest {
  std::string server;
};
struct MoveRequest {
  std::string server;
  std::vector<Shard> shards;
};
struct QueryRequest {};

// Responses
struct JoinResponse {};
struct LeaveResponse {};
struct MoveResponse {};
struct QueryResponse {
  ShardmasterConfig config;
};

// using SmRequest = std::variant<JoinRequest, LeaveRequest, MoveRequest,
// QueryRequest>; using SmResponse = std::variant<JoinResponse, LeaveResponse,
// MoveResponse, QueryResponse>;

#endif /* end of include guard */
