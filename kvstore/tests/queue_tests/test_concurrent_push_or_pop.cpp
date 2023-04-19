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
constexpr int N_PUSHERS = 8;
constexpr int N_POPPERS = 8;

void push(vector<int>& elts, synchronized_queue<int>& q) {
  for (auto&& i : elts) q.push(i);
}

void pop(synchronized_queue<int>& q, vector<int>& popped, int to_pop) {
  int cur;
  bool stopped;
  for (int i = 0; i < to_pop; i++) {
    stopped = q.pop(&cur);
    assert(!stopped);
    popped.push_back(cur);
  }
}

/**
 * Concurrently push N_ELTS / N_PUSHERS across N_PUSHERS threads, while
 * N_POPPERS threads attempt to pop that amount.
 */
int main() {
  synchronized_queue<int> q;

  // Create popper threads, and collect results
  vector<thread> popper_thrs(N_POPPERS);
  array<vector<int>, N_POPPERS> popped_elts;
  for (int i = 0; i < N_POPPERS; i++) {
    popper_thrs[i] =
        thread(pop, ref(q), ref(popped_elts[i]), N_ELTS / N_PUSHERS);
  }

  // generate thread chunks for pushing
  array<vector<int>, N_PUSHERS> chunks;
  vector<int> all_pushed(N_ELTS);
  for (int i = 0; i < N_ELTS; i++) {
    int r = rand() % MAX_ELT;
    chunks[i % N_PUSHERS].push_back(r);
    all_pushed[i] = r;
  }
  // Create pusher threads
  vector<thread> pusher_thrs(N_PUSHERS);
  for (int i = 0; i < N_PUSHERS; i++) {
    pusher_thrs[i] = thread(push, ref(chunks[i]), ref(q));
  }

  // Join pusher and popper threads, and accumulate elements
  for (auto&& thr : pusher_thrs) thr.join();
  vector<int> all_popped;
  all_popped.reserve(N_ELTS);
  for (int i = 0; i < N_POPPERS; i++) {
    popper_thrs[i].join();
    all_popped.insert(all_popped.end(), popped_elts[i].begin(),
                      popped_elts[i].end());
  }

  q.stop();

  // verify popped items' length
  assert(all_pushed.size() == all_popped.size());
  // sort and compare
  sort(all_pushed.begin(), all_pushed.end());
  sort(all_popped.begin(), all_popped.end());
  for (int i = 0; i < N_ELTS; i++) {
    assert(all_pushed[i] == all_popped[i]);
  }

  return 0;
}
