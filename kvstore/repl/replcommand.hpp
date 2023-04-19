#ifndef REPLCOMMAND_HPP
#define REPLCOMMAND_HPP

#include <sstream>
#include <string>

class ReplCommand {
 public:
  virtual void handle(const std::string& s) = 0;

  // Command information
  virtual std::string help() const {
    std::stringstream ss;
    ss << this->name() << " " << this->params() << ": " << this->description();
    return ss.str();
  };

  virtual std::string usage() const {
    std::stringstream ss;
    ss << "Usage: " << this->name() << " " << this->params();
    return ss.str();
  }

  virtual std::string name() const = 0;
  virtual std::string params() const = 0;
  virtual std::string description() const = 0;
};

#endif /* end of include guard */
