#include "test_utils/test_utils.hpp"

constexpr std::size_t kRandStringLength = 12;
constexpr std::size_t kNumKVPairs = 5;

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  auto keys = make_rand_strs(kNumKVPairs, kRandStringLength);
  auto vals = make_rand_strs(kNumKVPairs - 2, kRandStringLength);

  // MultiPut's key-value vector pair must be of the same size
  auto multiput_req = MultiPutRequest{.keys = keys, .values = vals};
  auto multiput_res = MultiPutResponse{};
  ASSERT(!store->MultiPut(&multiput_req, &multiput_res));
}
