#ifndef KVSTORE_HPP
#define KVSTORE_HPP

#include <map>
#include <string>
#include <vector>

#include "net/server_commands.hpp"

class KvStore {
 public:
  virtual ~KvStore() = default;

  virtual bool Get(const GetRequest* req, GetResponse* res) = 0;
  virtual bool Put(const PutRequest* req, PutResponse*) = 0;
  virtual bool Append(const AppendRequest* req, AppendResponse*) = 0;
  virtual bool Delete(const DeleteRequest* req, DeleteResponse* res) = 0;
  virtual bool MultiGet(const MultiGetRequest* req, MultiGetResponse* res) = 0;
  virtual bool MultiPut(const MultiPutRequest* req, MultiPutResponse*) = 0;

  virtual std::vector<std::string> AllKeys() = 0;
};

#endif /* end of include guard */
