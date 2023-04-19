#include "test_utils/test_utils.hpp"

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  std::string key = "hello";
  std::string val = "world";

  auto put_req = PutRequest{.key = key, .value = val};
  auto put_res = PutResponse{};
  ASSERT(store->Put(&put_req, &put_res));

  auto get_req = GetRequest{.key = key};
  auto get_res = GetResponse{};
  ASSERT(store->Get(&get_req, &get_res));
  ASSERT(get_res.value == val);
}
