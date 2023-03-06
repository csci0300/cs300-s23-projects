

#include <stdio.h>
#include "../io300.h"
// Copies contents of in-file into out-file byte by byte using io300_readc 
// and io300_writec. 

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "usage: %s <in-file> <out-file>\n", argv[0]);
        return 1;
    }
    struct io300_file *in = io300_open(argv[1], "\e[0;31min\e[0m");
    if (in == NULL) {
        return 1;
    }
    struct io300_file *out = io300_open(argv[2], "\e[0;32mout\e[0m");
    if (out == NULL) {
        io300_close(in);
        return 1;
    }

    off_t const filesize = io300_filesize(in);
    if (filesize <= 0) {
        fprintf(stderr, "error: could not compute filesize\n");
        io300_close(in);
        io300_close(out);
        return 1;
    }

    int exit_status = 0;
    for (int i = 0; i < filesize; i++) {
        int ch = io300_readc(in);
        if (ch == -1) {
            fprintf(stderr, "error: should not get EOF because we know the size of the file.\n");
            exit_status = 1;
            break;
        }
        if (io300_writec(out, ch) == -1) {
            fprintf(stderr, "error: write should not fail\n");
            exit_status = 1;
            break;
        }
    }

    io300_close(in);
    io300_close(out);
    return exit_status;
}