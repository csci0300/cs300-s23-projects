#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "test_utils/test_utils.hpp"

constexpr std::size_t kRandStringLength = 12;
constexpr std::size_t kNumIters = 10'000;

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  auto keys = make_rand_strs(kNumIters, kRandStringLength);
  auto vals = make_rand_strs(kNumIters, kRandStringLength);

  auto map = std::unordered_map<std::string, std::string>{};
  for (std::size_t i = 0; i < kNumIters; ++i) {
    auto put_req = PutRequest{.key = keys[i], .value = vals[i]};
    auto put_res = PutResponse{};
    store->Put(&put_req, &put_res);
    map[keys[i]] = vals[i];
  }

  for (std::size_t i = 0; i < kNumIters; ++i) {
    auto get_req = GetRequest{.key = keys[i]};
    auto get_res = GetResponse{};
    store->Get(&get_req, &get_res);
    ASSERT(get_res.value == vals[i]);
  }

  auto res = store->AllKeys();
  ASSERT(keys.size() == res.size());

  std::sort(keys.begin(), keys.end());
  std::sort(res.begin(), res.end());
  ASSERT(keys == res);
}
