#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "common/color.hpp"

class Client {
 public:
  virtual ~Client() = default;

  virtual std::optional<std::string> Get(const std::string& key) = 0;

  virtual bool Put(const std::string& key, const std::string& value) = 0;

  virtual bool Append(const std::string& key, const std::string& value) = 0;

  virtual std::optional<std::string> Delete(const std::string& key) = 0;

  virtual std::optional<std::vector<std::string>> MultiGet(
      const std::vector<std::string>& keys) = 0;

  virtual bool MultiPut(const std::vector<std::string>& keys,
                        const std::vector<std::string>& values) = 0;

  virtual bool GDPRDelete(const std::string& user) = 0;
};

#endif /* end of include guard */
