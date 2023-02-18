#include "dmalloc.hh"
#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
// heap_min and heap_max checking, no overlap with other regions.

static int global;

int main() {
    for (int i = 0; i != 100; ++i) {
        size_t sz = rand() % 100;
        char* p = (char*) malloc(sz);
        free(p);
    }
    dmalloc_stats stat;
    get_statistics(&stat);

    union {
        uintptr_t addr;
        int* iptr;
        dmalloc_stats* statptr;
        int (*mainptr)();
    } x;
    x.iptr = &global;
    assert(x.addr + sizeof(int) < stat.heap_min || x.addr >= stat.heap_max);
    x.statptr = &stat;
    assert(x.addr + sizeof(int) < stat.heap_min || x.addr >= stat.heap_max);
    x.mainptr = &main;
    assert(x.addr + sizeof(int) < stat.heap_min || x.addr >= stat.heap_max);
}
