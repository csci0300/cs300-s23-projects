#include "querycommand.hpp"

void QueryCommand::handle(const std::string&) {
  QueryRequest req;
  QueryResponse res;
  this->shardmaster->Query(&req, &res);

  std::cout << res.config.print();
}

std::string QueryCommand::name() const {
  return "query";
}

std::string QueryCommand::params() const {
  return "";
}

std::string QueryCommand::description() const {
  return "Gets the current shardmaster configuration.";
}
