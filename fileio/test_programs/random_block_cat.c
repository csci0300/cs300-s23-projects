

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../io300.h"

#define MAX_BLOCK_SIZE 30
// Copies the contents of in-file into out-file in blocks of random 
// sizes using io300_read and io300_write. 

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "usage: %s <in-file> <out-file>\n", argv[0]);
        return 1;
    }
    struct io300_file *in = io300_open(argv[1], "in");
    if (in == NULL) {
        return 1;
    }
    struct io300_file *out = io300_open(argv[2], "out");
    if (out == NULL) {
        io300_close(in);
        return 1;
    }

    char *const buffer = malloc(MAX_BLOCK_SIZE);
    if (buffer == NULL) {
        fprintf(stderr, "error: could not allocate buffer\n");
        io300_close(in);
        io300_close(out);
        return 1;
    }

    off_t const filesize = io300_filesize(in);
    if (filesize <= 0) {
        fprintf(stderr, "error: could not get filesize or filesize is 0\n");
        io300_close(in);
        io300_close(out);
        return 1;
    }


    int exit_status = 0;
    srand(strlen("cs300 rules"));

    while (1) {
        size_t const block_size = (rand() % MAX_BLOCK_SIZE) + 1;
        ssize_t const r = io300_read(in, buffer, block_size);
        if (r == 0 || r == -1) {
            // in file empty
            break;
        }

        if (io300_write(out, buffer, r) == -1) {
            fprintf(stderr, "error: all writes should succeed\n");
            exit_status = 1;
        }

    }

    free(buffer);
    io300_close(in);
    io300_close(out);
    return exit_status;
}