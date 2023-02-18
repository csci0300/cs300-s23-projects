#include "dmalloc.hh"
#include <cstdio>
#include <cassert>
#include <cstring>
// Memory leak report with no leaks.

struct node {
    node* next;
};

int main() {
    node* list = nullptr;

    // create a list
    for (int i = 0; i != 400; ++i) {
        node* n = (node*) malloc(sizeof(node));
        n->next = list;
        list = n;
    }

    // free everything in it
    while (node* n = list) {
        list = n->next;
        free(n);
    }

    print_leak_report();
    printf("OK\n");
}

//! OK
