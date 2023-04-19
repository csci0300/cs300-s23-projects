#ifndef SHARDKVCLIENT_GETCOMMAND_HPP
#define SHARDKVCLIENT_GETCOMMAND_HPP

#include <iostream>
#include <memory>
#include <sstream>

#include "client.hpp"
#include "common/color.hpp"
#include "common/utils.hpp"
#include "repl/replcommand.hpp"
#include "shardkv_client.hpp"

class GetCommand : public ReplCommand {
 public:
  explicit GetCommand(std::shared_ptr<Client> c) : client(c) {
  }

  void handle(const std::string& s) override;

  std::string name() const override;
  std::string params() const override;
  std::string description() const override;

 private:
  std::shared_ptr<Client> client;
};

#endif /* end of include guard */
