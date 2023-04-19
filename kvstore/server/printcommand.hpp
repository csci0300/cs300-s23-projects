#ifndef SERVER_PRINTCOMMAND_HPP
#define SERVER_PRINTCOMMAND_HPP

#include <memory>
#include <sstream>

#include "common/utils.hpp"
#include "repl/replcommand.hpp"
#include "server.hpp"

class PrintCommand : public ReplCommand {
 public:
  explicit PrintCommand(std::shared_ptr<KvServer> s) : server(s) {
  }

  void handle(const std::string& s) override;

  std::string name() const override;
  std::string params() const override;
  std::string description() const override;

 private:
  std::shared_ptr<KvServer> server;
};

#endif /* end of include guard */
