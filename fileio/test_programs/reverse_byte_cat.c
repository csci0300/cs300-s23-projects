

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../io300.h"
// Copies the contents of in-file into out-file reversed byte 
// by byte using io300_seek, io300_readc and io300_writec. 

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

    off_t const filesize = io300_filesize(in);
    if (filesize <= 0) {
        fprintf(stderr, "error: trouble getting filesize or empty input file\n");
        return 1;
    }

    for (int i = filesize - 1; i >= 0; i--) {
        if (io300_seek(in, i) == -1) {
            fprintf(stderr, "error: seek should not fail\n");
            io300_close(in);
            io300_close(out);
            return 1;
        }
        int ch = io300_readc(in);
        if (ch == -1) {
            fprintf(stderr, "error: read should not fail.\n");
            io300_close(in);
            io300_close(out);
            return 1;
        }
        if (io300_writec(out, ch) == -1) {
            fprintf(stderr, "error: write should not fail.\n");
            io300_close(in);
            io300_close(out);
            return 1;
        }
    }

    io300_close(in);
    io300_close(out);
    return 0;
}