#include <cassert>
#include <vector>

#include "server/synchronized_queue.hpp"

// for simplicity
using namespace std;

constexpr int N_ELTS = 131000;

/**
 * Simple (serial) push N_ELTS, then pop N_ELTS. Use a single thread to first
 * push and then pop elements in order.
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

  // pop all elements
  int cur;
  bool stopped;
  for (int i = 0; i < N_ELTS; i++) {
    stopped = q.pop(&cur);
    assert(!stopped);
    assert(cur == i);
  }

  q.stop();
  return 0;
}
