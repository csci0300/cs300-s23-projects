#include <cassert>
#include <vector>

#include "server/synchronized_queue.hpp"

// for simplicity
using namespace std;

constexpr int N_ELTS = 131000;

/**
 * Simple (serial) push, then pop, N_ELTS. Use a single thread to alternatively
 * push and pop elements in order.
 */
int main() {
  synchronized_queue<int> q;

  size_t s;
  int cur;
  bool stopped;
  // push, then pop, every element

  for (int i = 0; i < N_ELTS; i++) {
    // push, check size
    q.push(i);
    s = q.size();
    assert(s == 1);

    // pop, check stopped, elt, size
    stopped = q.pop(&cur);
    assert(!stopped);
    assert(cur == i);

    s = q.size();
    assert(s == 0);
  }

  q.stop();
  return 0;
}
