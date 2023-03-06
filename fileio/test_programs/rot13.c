

#include <stdio.h>
#include "../io300.h"
// Uses ROT13, a simple cipher, to obscure the contents of 
// in-file character by character. 

unsigned char rot13(unsigned char const ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return 'A' + (((ch - 'A') + 13) % 26);
    }
    if (ch >= 'a' && ch <= 'z') {
        return 'a' + (((ch - 'a') + 13) % 26);
    }
    return ch;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "usage: %s <in-file>\n", argv[0]);
        return 1;
    }
    struct io300_file *in = io300_open(argv[1], "\e[0;31min\e[0m");
    if (in == NULL) {
        return 1;
    }


    off_t const filesize = io300_filesize(in);
    if (filesize == -1) {
        fprintf(stderr, "error: could not compute filesize\n");
        io300_close(in);
        return 1;
    }
    for (int i = 0; i < filesize; i++) {
        if (io300_seek(in, i) == -1) {
            fprintf(stderr, "error: seek should not fail.\n");
            io300_close(in);
            return 1;
        }
        int ch = io300_readc(in);
        if (ch == -1) {
            fprintf(stderr, "error: read should not fail.\n");
            io300_close(in);
            return 1;
        }
        if (io300_seek(in, i) == -1) {
            fprintf(stderr, "error: seek should not fail.\n");
            io300_close(in);
            return 1;
        }

        if (io300_writec(in, rot13(ch)) == -1) {
            fprintf(stderr, "error: write should not fail.\n");
            io300_close(in);
            return 1;
        }
    }

    io300_close(in);

    return 0;
}