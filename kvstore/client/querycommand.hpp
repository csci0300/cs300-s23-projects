#ifndef SHARDKVCLIENT_QUERYCOMMAND_HPP
#define SHARDKVCLIENT_QUERYCOMMAND_HPP

#include <memory>
#include <sstream>

#include "repl/replcommand.hpp"
#include "shardkv_client.hpp"

class QueryCommand : public ReplCommand {
 public:
  explicit QueryCommand(std::shared_ptr<ShardKvClient> c) : client(c) {
  }

  void handle(const std::string&) override;

  std::string name() const override;
  std::string params() const override;
  std::string description() const override;

 private:
  std::shared_ptr<ShardKvClient> client;
};

#endif /* end of include guard */
