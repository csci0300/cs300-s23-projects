#ifndef SHARDMASTER_QUERYCOMMAND_HPP
#define SHARDMASTER_QUERYCOMMAND_HPP

#include <memory>
#include <sstream>

#include "repl/replcommand.hpp"
#include "shardmaster.hpp"

class QueryCommand : public ReplCommand {
 public:
  explicit QueryCommand(std::shared_ptr<Shardmaster> sm) : shardmaster(sm) {
  }

  void handle(const std::string&) override;

  std::string name() const override;
  std::string params() const override;
  std::string description() const override;

 private:
  std::shared_ptr<Shardmaster> shardmaster;
};

#endif /* end of include guard */
