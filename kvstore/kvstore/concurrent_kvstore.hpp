#ifndef CONCURRENT_KVSTORE_HPP
#define CONCURRENT_KVSTORE_HPP

#include <array>
#include <cassert>
#include <list>
#include <map>
#include <optional>
#include <shared_mutex>
#include <string>

#include "common/utils.hpp"
#include "kvstore.hpp"
#include "net/server_commands.hpp"

/**
 * Struct encapsulating a database item. This is optional, but you may find this
 * helpful in your DbMap implementation.
 */
struct DbItem {
  std::string key;
  std::string value;

  DbItem(std::string& k, std::string& v) {
    this->key = k;
    this->value = v;
  }

  bool operator==(const DbItem& item) {
    return (this->key == item.key && this->value == item.value);
  }
};

/**
 * Implement your bucket-based map here!
 */
class DbMap {
 public:
  DbMap() = default;

  static constexpr size_t BUCKET_COUNT = 60;

  // Bucket associative array, with corresponding mutexes to protect access.
  std::array<std::list<DbItem>, BUCKET_COUNT> buckets;

  // TODO (Part A, Step 5): You will need to add fields to synchronize access to
  // the hashmap buckets!

  // Return the index of the bucket to search for `key`.
  size_t bucket(std::string key) const {
    return hash(key) % BUCKET_COUNT;
  }

  // Returns the DbItem with key 'key' in bucket `b` if it exists, std::nullopt
  // otherwise Assumes that `b` == this->bucket(key).
  std::optional<DbItem> getIfExists(size_t b, std::string key) {
    assert(b < BUCKET_COUNT);
    for (const auto& item : this->buckets[b]) {
      if (item.key == key) {
        return item;
      }
    }
    return std::nullopt;
  }

  // Insert a new DbItem with key 'key' and value 'value' to bucket `b`.
  // If key already exists, updates value to `value`.
  // Assumes that `b` == this->bucket(key).
  void insertItem(size_t b, std::string key, std::string value) {
    assert(b < BUCKET_COUNT);

    for (auto& item : this->buckets[b]) {
      if (item.key == key) {
        item.value = value;
        return;
      }
    }
    this->buckets[b].emplace_back(key, value);
  }

  // Remove a DbItem with key `key` from bucket `b`.
  // Assumes that `b` == this->getBucketIndex(key).
  bool removeItem(size_t b, std::string key) {
    assert(b < BUCKET_COUNT);

    size_t num_removed = this->buckets[b].remove_if(
        [&](auto&& item) { return item.key == key; });
    return num_removed > 0;
  }
};

class ConcurrentKvStore : public KvStore {
 public:
  ConcurrentKvStore() = default;
  ~ConcurrentKvStore() = default;

  bool Get(const GetRequest* req, GetResponse* res) override;
  bool Put(const PutRequest* req, PutResponse* res) override;
  bool Append(const AppendRequest* req, AppendResponse* res) override;
  bool Delete(const DeleteRequest* req, DeleteResponse* res) override;
  bool MultiGet(const MultiGetRequest* req, MultiGetResponse* res) override;
  bool MultiPut(const MultiPutRequest* req, MultiPutResponse* res) override;

  std::vector<std::string> AllKeys() override;

 private:
  // Your internal key-value store implementation!
  DbMap store;
};

#endif /* end of include guard */
