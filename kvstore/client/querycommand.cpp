#include "querycommand.hpp"

void QueryCommand::handle(const std::string&) {
  auto config = this->client->Query();
  if (!config) {
    cerr_color(RED, "Failed to query shardmaster.");
    return;
  }

  std::cout << config->print();
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
