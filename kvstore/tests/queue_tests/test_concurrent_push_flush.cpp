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
constexpr int N_THREADS = 16;

void push(vector<int>& elts, synchronized_queue<int>& q) {
  for (auto&& i : elts) q.push(i);
}

void flush_thr(synchronized_queue<int>& q, vector<int>& flushed) {
  flushed = q.flush();
}

/**
 * Concurrently push N_ELTS / N_THREADS, with interleaved flushes.
 */
int main() {
  synchronized_queue<int> q;

  // generate thread chunks for pushing
  array<vector<int>, N_THREADS> chunks;
  vector<int> all_pushed(N_ELTS);
  for (int i = 0; i < N_ELTS; i++) {
    int r = rand() % MAX_ELT;
    chunks[i % N_THREADS].push_back(r);
    all_pushed[i] = r;
  }
  // Alternate pusher and flusher threads.
  vector<thread> thrs(N_THREADS * 2);
  array<vector<int>, N_THREADS> flushed_elts;
  for (int i = 0; i < N_THREADS * 2; i++) {
    if (i % 2 == 0)
      thrs[i] = thread(flush_thr, ref(q), ref(flushed_elts[i / 2]));
    else
      thrs[i] = thread(push, ref(chunks[i / 2]), ref(q));
  }

  // Join threads, and accumulate flushed values
  for (auto&& thr : thrs) thr.join();
  q.stop();

  // Accumulate flushed elements
  vector<int> all_flushed = q.flush();
  all_flushed.reserve(N_ELTS);
  for (int i = 0; i < N_THREADS; i++) {
    all_flushed.insert(all_flushed.end(), flushed_elts[i].begin(),
                       flushed_elts[i].end());
  }

  q.stop();

  // verify popped items' length
  assert(all_pushed.size() == all_flushed.size());
  // sort and compare
  sort(all_pushed.begin(), all_pushed.end());
  sort(all_flushed.begin(), all_flushed.end());
  for (int i = 0; i < N_ELTS; i++) {
    assert(all_pushed[i] == all_flushed[i]);
  }

  return 0;
}
