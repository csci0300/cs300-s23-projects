#ifndef CLIENT_GDPRDELETECOMMAND_HPP
#define CLIENT_GDPRDELETECOMMAND_HPP

#include <memory>
#include <sstream>

#include "client.hpp"
#include "common/utils.hpp"
#include "repl/replcommand.hpp"

class GDPRDeleteCommand : public ReplCommand {
 public:
  explicit GDPRDeleteCommand(std::shared_ptr<Client> c) : client(c) {
  }

  void handle(const std::string& user) override;

  std::string name() const override;
  std::string params() const override;
  std::string description() const override;

 private:
  std::shared_ptr<Client> client;
};

#endif /* end of include guard */
