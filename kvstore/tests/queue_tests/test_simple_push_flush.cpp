#include <cassert>
#include <vector>

#include "server/synchronized_queue.hpp"

// for simplicity
using namespace std;

constexpr int N_ELTS = 131000;

/**
 * Simple (serial) push, then flush. Use a single thread to first push, then
 * flush the elements.
 */
int main() {
  synchronized_queue<int> q;

  size_t s;
  // push all elements
  for (int i = 0; i < N_ELTS; i++) {
    q.push(i);
  }

  s = q.size();
  assert(s == N_ELTS);

  // flush elements
  vector<int> elts = q.flush();
  assert(elts.size() == N_ELTS);

  for (int i = 0; i < N_ELTS; i++) {
    assert(elts[i] == i);
  }

  q.stop();
  return 0;
}
