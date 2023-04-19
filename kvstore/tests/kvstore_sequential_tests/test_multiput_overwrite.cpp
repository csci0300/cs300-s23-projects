#include "test_utils/test_utils.hpp"

constexpr std::size_t kRandStringLength = 12;
constexpr std::size_t kFirstMultiPutSize = 16;
constexpr std::size_t kSecondMultiPutSize = 20;

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  auto keys = make_rand_strs(kFirstMultiPutSize, kRandStringLength);
  auto vals = make_rand_strs(kFirstMultiPutSize, kRandStringLength);

  auto multiput_req = MultiPutRequest{.keys = keys, .values = vals};
  auto multiput_res = MultiPutResponse{};
  ASSERT(store->MultiPut(&multiput_req, &multiput_res));

  // the second MultiPut request updates some key-value pairs from the first
  // insertion, and adds some new key-value pairs.
  auto second_put_vals = make_rand_strs(kSecondMultiPutSize, kRandStringLength);
  auto second_put_keys =
      std::vector(keys.begin(), keys.begin() + kFirstMultiPutSize / 2);
  for (std::size_t i = kFirstMultiPutSize / 2; i < kSecondMultiPutSize; ++i) {
    second_put_keys.push_back(random_string(kRandStringLength));
  }

  multiput_req.keys = second_put_keys;
  multiput_req.values = second_put_vals;
  ASSERT(store->MultiPut(&multiput_req, &multiput_res));

  // when requesting for the keys from the first MultiPut request, we expect
  // part of the mapped values to have been updated.
  auto expected_vals =
      std::vector(second_put_vals.begin(),
                  second_put_vals.begin() + kFirstMultiPutSize / 2);
  expected_vals.insert(expected_vals.end(),
                       vals.begin() + kFirstMultiPutSize / 2, vals.end());

  auto multiget_req = MultiGetRequest{.keys = keys};
  auto multiget_res = MultiGetResponse{};
  ASSERT(store->MultiGet(&multiget_req, &multiget_res));
  ASSERT(check_equality(multiget_res.values, expected_vals));
}
