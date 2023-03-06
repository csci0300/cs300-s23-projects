

#include <stdio.h>
#include <stdlib.h>
#include "../io300.h"
// Copies the contents of in-file into out-file reversed in blocks of 
// buffer-size using io300_seek, io300_read and io300_write. 

int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "usage: %s <buffer-size> <in-file> <out-file>\n", argv[0]);
        return 1;
    }
    int const buffer_size = atoi(argv[1]);
    if (buffer_size == 0) {
        fprintf(stderr, "error: specify numeric buffer size > 0 bytes. you gave %s\n", argv[1]);
        return 1;
    }
    struct io300_file *in = io300_open(argv[2], "in");
    if (in == NULL) {
        return 1;
    }
    struct io300_file *out = io300_open(argv[3], "out");
    if (out == NULL) {
        io300_close(in);
        return 1;
    }

    off_t const filesize = io300_filesize(in);
    if (filesize <= 0) {
        fprintf(stderr, "error: could not get filesize or empty file\n");
        io300_close(in);
        io300_close(out);
        return 1;
    }

    char *const in_buffer = malloc(buffer_size);
    if (in_buffer == NULL) {
        fprintf(stderr, "error: could not allocate buffer\n");
        io300_close(in);
        io300_close(out);
        return 1;
    }

    char *const out_buffer = malloc(buffer_size);
    if (out_buffer == NULL) {
        fprintf(stderr, "error: could not allocate out buffer\n");
        free(in_buffer);
        io300_close(in);
        io300_close(out);
        return 1;
    }

    int exit_status = 0;

    for (int i = filesize - (filesize % buffer_size); i >= 0; i -= buffer_size) {
        if (io300_seek(in, i) == -1) {
            fprintf(stderr, "error: seek should not fail\n");
            exit_status = 1;
            break;
        }
        int const n = io300_read(in, in_buffer, buffer_size);
        if (n == -1) {
            break;
        }
        for (int j = 0; j < n; j++) {
            out_buffer[j] = in_buffer[(n - 1) - j];
        }
        if (io300_write(out, out_buffer, n) == -1) {
            fprintf(stderr, "error: write should not fail\n");
            exit_status = 1;
            break;
        }
    }

    io300_close(in);
    io300_close(out);
    free(in_buffer);
    free(out_buffer);

    return exit_status;
}