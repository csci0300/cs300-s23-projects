#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <thread>
#include <vector>

#include "server/synchronized_queue.hpp"

// for simplicity
using namespace std;

constexpr int N_ELTS = 160000;
constexpr int MAX_ELT = 1310;
constexpr int N_THREADS = 8;

void pop(synchronized_queue<int>& q) {
  int cur;
  bool stopped = q.pop(&cur);
  assert(stopped);
}

/**
 * Try to pop from queue that is stopped.
 */
int main() {
  synchronized_queue<int> q;

  // Create threads
  vector<thread> thrs(N_THREADS);
  for (int i = 0; i < N_THREADS; i++) {
    thrs[i] = thread(pop, ref(q));
  }

  q.stop();

  for (auto&& thr : thrs) thr.join();

  return 0;
}
