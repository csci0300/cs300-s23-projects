
#include <array>
#include <future>
#include <iostream>
#include <string>

#include "test_utils/test_utils.hpp"

static constexpr std::size_t kRandStringLength = 32;
static constexpr std::size_t kNumThreads = 8;
static constexpr std::size_t kNumKeyValPairs = 32;
static constexpr std::size_t kNumMultiOps = 4'000;

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  // Generate random key-value pairs
  auto keys = make_rand_strs(kNumKeyValPairs, kRandStringLength);
  auto n_threads = kNumThreads;

  auto threads = std::vector<std::future<bool>>{};
  char c = 'A';
  // First, insert all 'A's into the store.
  std::vector<std::string> vals(kNumKeyValPairs, std::string(1, c));
  auto start = 0;
  auto end = kNumKeyValPairs;
  ASSERT(multiput_range(*store, keys, vals, start, end, kNumKeyValPairs));

  // Now, half of the threads should be MultiPutting the keys; the other half
  // should be MultiGetting. Each MultiGetter should only get one value.
  for (std::size_t i = 0; i < n_threads; ++i) {
    // Each thread should try to put all of the keys
    threads.push_back(std::async(std::launch::async, [&, tid = i]() {
      // putter
      if (tid % 2) {
        std::vector<std::string> vals(kNumKeyValPairs, std::string(1, c + tid));
        auto start = 0;
        auto end = kNumKeyValPairs;
        for (std::size_t j = 0; j < kNumMultiOps / 2; j++) {
          ASSERT(
              multiput_range(*store, keys, vals, start, end, kNumKeyValPairs));
        }
        return true;
      } else {
        // getter
        for (std::size_t j = 0; j < kNumMultiOps; j++) {
          auto mget_req = MultiGetRequest{keys};
          auto mget_res = MultiGetResponse{};
          ASSERT(store->MultiGet(&mget_req, &mget_res));
          // All values should be the same (one of A -> H)
          std::string end_val = mget_res.values[0];
          for (auto&& v : mget_res.values) {
            ASSERT_EQ(end_val, v);
          }
        }
        return true;
      }
    }));
  }

  auto passed = true;
  for (auto& t : threads) {
    passed &= t.get();
  }
  ASSERT(passed);

  return 0;
}
