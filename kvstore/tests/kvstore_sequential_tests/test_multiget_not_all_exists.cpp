#include "test_utils/test_utils.hpp"

constexpr std::size_t kRandStringLength = 12;
constexpr std::size_t kNumKVPairs = 5;

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  auto keys = make_rand_strs(kNumKVPairs, kRandStringLength);
  auto vals = make_rand_strs(kNumKVPairs, kRandStringLength);

  // insert all but the last key-value pair
  auto put_req = PutRequest{};
  auto put_res = PutResponse{};
  for (std::size_t i = 0; i < kNumKVPairs - 1; ++i) {
    put_req.key = keys[i];
    put_req.value = vals[i];
    ASSERT(store->Put(&put_req, &put_res));
  }

  // MultiGet should fail because the last key was not inserted
  auto multiget_req = MultiGetRequest{.keys = keys};
  auto multiget_res = MultiGetResponse{};
  ASSERT(!store->MultiGet(&multiget_req, &multiget_res));
}
