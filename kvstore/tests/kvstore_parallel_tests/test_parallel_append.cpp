#include <array>
#include <future>
#include <iostream>
#include <string>

#include "test_utils/test_utils.hpp"

static constexpr std::size_t kRandStringLength = 32;
static constexpr std::size_t kNumThreads = 8;
static constexpr std::size_t kNumKeyValPairs = 10'000;
static constexpr std::size_t kNumToAppend = 10;

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  // Generate random keys
  auto keys = make_rand_strs(kNumKeyValPairs, kRandStringLength);

  // Have threads append A -> (A + kNumThreads)
  char c = 'A';
  auto n_threads = kNumThreads;
  auto threads = std::vector<std::future<bool>>{};
  for (std::size_t i = 0; i < n_threads; ++i) {
    threads.push_back(std::async(std::launch::async, [&, tid = i]() {
      std::string to_append = std::string(1, c + tid);
      // Append (A + i) kNumToAppend times
      for (std::size_t i = 0; i < kNumKeyValPairs; i++) {
        for (std::size_t j = 0; j < kNumToAppend; j++) {
          auto append_req = AppendRequest{.key = keys[i], .value = to_append};
          auto append_res = AppendResponse{};
          ASSERT(store->Append(&append_req, &append_res));
        }
      }
      return true;
    }));
  }

  auto passed = true;
  for (auto& t : threads) {
    passed &= t.get();
  }
  ASSERT(passed);

  // For each key, get its value
  for (auto&& k : keys) {
    auto get_req = GetRequest{k};
    auto get_res = GetResponse{};
    ASSERT(store->Get(&get_req, &get_res));

    auto val = get_res.value;
    // Check that we have kNumToAppend for each character
    std::map<char, std::size_t> counts;
    for (auto&& c : val) {
      counts[c] += 1;
    }
    ASSERT_EQ(counts.size(), kNumThreads);
    for (auto&& [_, c] : counts) {
      ASSERT_EQ(c, kNumToAppend);
    }
  }
}
