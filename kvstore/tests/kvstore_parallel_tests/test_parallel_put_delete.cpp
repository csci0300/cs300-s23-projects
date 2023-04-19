#include <array>
#include <future>
#include <iostream>
#include <string>

#include "test_utils/test_utils.hpp"

static constexpr std::size_t kRandStringLength = 32;
static constexpr std::size_t kNumThreads = 8;
static constexpr std::size_t kNumKeyValPairs = 10'000;

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  // Generate random key-value pairs
  auto keys = make_rand_strs(kNumKeyValPairs, kRandStringLength);
  auto vals = make_rand_strs(kNumKeyValPairs, kRandStringLength);

  // In this test, we first insert the first half of the key-value pairs into
  // the store. Next, N threads concurrently insert and remove from the store,
  // where:
  // - the first N/2 threads remove elements already inserted into the store
  // - the second N/2 threads insert yet-to-be-inserted key-value pairs.

  auto preinsert_start = 0;
  auto preinsert_end = kNumKeyValPairs / 2;
  put_range(*store, keys, vals, preinsert_start, preinsert_end);

  auto n_threads = kNumThreads;
  auto elems_per_thr = kNumKeyValPairs / n_threads;
  auto threads = std::vector<std::future<bool>>{};
  for (std::size_t i = 0; i < n_threads; ++i) {
    threads.push_back(std::async(std::launch::async, [&, tid = i]() {
      auto start = tid * elems_per_thr;
      auto end = start + elems_per_thr;

      auto is_deleter = tid < (n_threads / 2);
      if (is_deleter) {
        return del_range(*store, keys, vals, start, end);
      } else {
        return put_range(*store, keys, vals, start, end);
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
