#include "test_utils/test_utils.hpp"

constexpr std::size_t kRandStringLength = 12;

void test_multiput_multiget(std::unique_ptr<KvStore> store,
                            std::size_t num_kv_pairs) {
  auto keys = make_rand_strs(num_kv_pairs, kRandStringLength);
  auto vals = make_rand_strs(num_kv_pairs, kRandStringLength);

  auto multiput_req = MultiPutRequest{.keys = keys, .values = vals};
  auto multiput_res = MultiPutResponse{};
  ASSERT(store->MultiPut(&multiput_req, &multiput_res));

  auto multiget_req = MultiGetRequest{.keys = keys};
  auto multiget_res = MultiGetResponse{};
  ASSERT(store->MultiGet(&multiget_req, &multiget_res));
  ASSERT(multiget_res.values == vals);
}

void test_small_multiput_multiget(std::unique_ptr<KvStore> store) {
  constexpr std::size_t kNumKVPairs = 1 << 4;  // 2^4
  test_multiput_multiget(std::move(store), kNumKVPairs);
}

void test_big_multiput_multiget(std::unique_ptr<KvStore> store) {
  constexpr std::size_t kNumKVPairs = 1 << 12;  // 2^12
  test_multiput_multiget(std::move(store), kNumKVPairs);
}

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);
  TEST(test_small_multiput_multiget, std::move(store));

  store = make_kvstore(argc, argv);
  TEST(test_big_multiput_multiget, std::move(store));

  return 0;
}
