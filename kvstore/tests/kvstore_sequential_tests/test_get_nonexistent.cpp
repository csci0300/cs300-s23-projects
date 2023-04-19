#include "test_utils/test_utils.hpp"

int main(int argc, char* argv[]) {
  auto store = make_kvstore(argc, argv);

  auto req = GetRequest{"nonexistent"};
  auto res = GetResponse{};
  ASSERT(!store->Get(&req, &res));
}
