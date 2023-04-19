#ifndef SHARDKVCLIENT_LEAVECOMMAND_HPP
#define SHARDKVCLIENT_LEAVECOMMAND_HPP

#include <algorithm>
#include <memory>
#include <sstream>

#include "common/utils.hpp"
#include "repl/replcommand.hpp"
#include "shardkv_client.hpp"

class LeaveCommand : public ReplCommand {
 public:
  explicit LeaveCommand(std::shared_ptr<ShardKvClient> c) : client(c) {
  }

  void handle(const std::string& s) override;

  std::string name() const override;
  std::string params() const override;
  std::string description() const override;

 private:
  std::shared_ptr<ShardKvClient> client;
};

#endif /* end of include guard */
