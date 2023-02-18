#include "dmalloc.hh"
#include <cstdio>
#include <cassert>
#include <cstring>
// Free of invalid pointer.

int main() {
    free((void*) 16);
    print_statistics();
}

//! MEMORY BUG???: invalid free of pointer ???, not in heap
//! ???
