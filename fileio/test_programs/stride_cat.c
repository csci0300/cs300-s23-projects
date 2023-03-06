#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../io300.h"
//    Copies the input INFILE to OUTFILE in blocks, shuffling its
//    contents. Reads INFILE in a strided access pattern and writes
//    to OUTFILE sequentially. For example, if BLOCKSIZE is 1 and STRIDE 
//    is 1024, the input file's bytes are read in the sequence
//    0, 1024, 2048, ..., 1, 1025, 2049, ..., etc.

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "usage: %s <BLOCKSIZE> <STRIDE> <INFILE> <OUTFILE>\n", argv[0]);
        return 1;
    }

    size_t block_size = strtol(argv[1], NULL, 10);

    // Allocate buffer, open files, measure file sizes
    char* buf = malloc(block_size);

    struct io300_file* inf = io300_open(argv[3], "\e[0;31min\e[0m");

    size_t input_size = io300_filesize(inf);
    if ((ssize_t) input_size < 0) {
        fprintf(stderr, "stride_cat: can't get size of input file\n");
        exit(1);
    }
    if (io300_seek(inf, 0) < 0) {
        fprintf(stderr, "stride_cat: input file is not seekable\n");
        exit(1);
    }

    struct io300_file* outf = io300_open(argv[4], "\e[0;32mout\e[0m");

    size_t stride = strtol(argv[2], NULL, 10);

    // Copy file data
    size_t pos = 0, written = 0;
    while (written < input_size) {
        // Copy a block
        ssize_t amount = io300_read(inf, buf, block_size);
        if (amount <= 0) {
            break;
        }
        io300_write(outf, buf, amount);
        written += amount;

        // Move `inf` file position to next stride
        pos += stride;
        if (pos >= input_size) {
            pos = (pos % stride) + block_size;
            if (pos + block_size > stride) {
                block_size = stride - pos;
            }
        }
        int r = io300_seek(inf, pos);
        assert(r >= 0);
    }

    io300_close(inf);
    io300_close(outf);
    free(buf);
}
