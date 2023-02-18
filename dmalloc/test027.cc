#include "dmalloc.hh"
#include <cstdio>
#include <cassert>
#include <cstring>
// More boundary write error checks #2.

int main() {
    int* array = (int*) malloc(3); // oops, forgot "* sizeof(int)"
    for (int i = 0; i != 3; ++i) {
        array[i] = 0;
    }
    free(array);
    print_statistics();
}

//! MEMORY BUG???: detected wild write during free of pointer ???
//! ???
