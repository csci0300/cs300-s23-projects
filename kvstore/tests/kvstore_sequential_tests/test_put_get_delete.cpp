#include <iostream>
#include <string>
#include <unordered_map>

#include "test_utils/test_utils.hpp"

constexpr std::size_t kRandStringLength = 12;
constexpr std::size_t kNumKeyValPairs = 10'000;

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  auto keys = make_rand_strs(kNumKeyValPairs, kRandStringLength);
  auto vals = make_rand_strs(kNumKeyValPairs, kRandStringLength);

  for (std::size_t i = 0; i < kNumKeyValPairs; ++i) {
    auto put_req = PutRequest{.key = keys[i], .value = vals[i]};
    auto put_res = PutResponse{};
    store->Put(&put_req, &put_res);
  }

  for (std::size_t i = 0; i < kNumKeyValPairs; i++) {
    auto get_req = GetRequest{.key = keys[i]};
    auto get_res = GetResponse{};
    ASSERT(store->Get(&get_req, &get_res));
    ASSERT_EQ(get_res.value, vals[i]);
  }

  for (std::size_t i = 0; i < kNumKeyValPairs; i++) {
    auto del_req = DeleteRequest{keys[i]};
    auto del_res = DeleteResponse{};
    ASSERT(store->Delete(&del_req, &del_res));
    ASSERT(del_res.value == vals[i]);
  }
}
