#include "test_utils/test_utils.hpp"

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  auto features = std::vector<std::string>{"concepts,", "modules,",
                                           "coroutines,", "ranges,", "jthread"};
  std::string key = "c++20features";
  std::string val = features[0];

  auto put_req = PutRequest{.key = key, .value = val};
  auto put_res = PutResponse{};
  ASSERT(store->Put(&put_req, &put_res));

  for (std::size_t i = 1; i < features.size(); ++i) {
    auto append_req = AppendRequest{.key = key, .value = features[i]};
    auto append_res = AppendResponse{};
    ASSERT(store->Append(&append_req, &append_res));
  }

  std::string expected =
      std::accumulate(features.begin(), features.end(), std::string(""));

  auto get_req = GetRequest{.key = key};
  auto get_res = GetResponse{};
  ASSERT(store->Get(&get_req, &get_res));
  ASSERT(get_res.value == expected);
}
