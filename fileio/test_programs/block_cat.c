

#include <stdio.h>
#include <stdlib.h>
#include "../io300.h"
// Copies contents of in-file into out-file in blocks of buffer-size using 
// io300_read and io300_write. 

int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "usage: %s <buffer-size> <in-file> <out-file>\n", argv[0]);
        return 1;
    }
    int const buffer_size = atoi(argv[1]);
    if (buffer_size == 0) {
        fprintf(stderr, "error: specify numeric buffer size > 0 bytes. you gave %s\n", argv[1]);
    }
    struct io300_file *in = io300_open(argv[2], "\e[0;31min\e[0m");
    if (in == NULL) {
        return 1;
    }
    struct io300_file *out = io300_open(argv[3], "\e[0;32mout\e[0m");
    if (out == NULL) {
        io300_close(in);
        return 1;
    }

    char *const buffer = malloc(buffer_size);
    if (buffer == NULL) {
        fprintf(stderr, "error: could not allocate buffer\n");
        io300_close(in);
        io300_close(out);
        return 1;
    }

    int exit_status = 0;
    while (1) {
        ssize_t const r = io300_read(in, buffer, buffer_size);
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