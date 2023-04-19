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

void push_then_pop(vector<int>& elts, synchronized_queue<int>& q,
                   vector<int>& popped) {
  int cur;
  bool stopped;
  // push, then pop
  for (auto&& i : elts) {
    q.push(i);
    stopped = q.pop(&cur);
    assert(!stopped);
    popped.push_back(cur);
  }
}

/**
 * Concurrently push, then pop, N_ELTS / N_THREADS across N_THREADS.
 */
int main() {
  synchronized_queue<int> q;

  // generate thread chunks
  array<vector<int>, N_THREADS> chunks;
  vector<int> all_pushed(N_ELTS);
  for (int i = 0; i < N_ELTS; i++) {
    int r = rand() % MAX_ELT;
    chunks[i % N_THREADS].push_back(r);
    all_pushed[i] = r;
  }

  // Create threads, and collect results
  vector<thread> thrs(N_THREADS);
  array<vector<int>, N_THREADS> popped_elts;
  for (int i = 0; i < N_THREADS; i++) {
    thrs[i] =
        thread(push_then_pop, ref(chunks[i]), ref(q), ref(popped_elts[i]));
  }

  // Join threads, and accumulate elements
  vector<int> all_popped;
  all_popped.reserve(N_ELTS);
  for (int i = 0; i < N_THREADS; i++) {
    thrs[i].join();
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
