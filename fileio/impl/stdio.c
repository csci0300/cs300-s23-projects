
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "../io300.h"


/*
    stdio.c

    This implementation wraps the stdio cached read/write calls.
*/


struct io300_file {
    /* stdio calls require a FILE *, not a file descriptor */
    FILE *f;
};


struct io300_file *io300_open(const char *path, char *name) {
    struct io300_file *const ret = malloc(sizeof(*ret));
    if (ret) {
        ret->f = fopen(path, "r+");
        if (ret->f == NULL) {
            fprintf(stderr, "error: could not open file: %s\n", path);
            free(ret);
            return NULL;
        }
    } else {
        fprintf(stderr, "error: could not allocate io300_file\n");
    }
    return ret;
}

int io300_close(struct io300_file *f) {
    io300_flush(f);
    int const ret = fclose(f->f);
    free(f);
    return ret;
}

off_t io300_filesize(struct io300_file *f) {
    struct stat s;
    int const r = fstat(fileno(f->f), &s);
    if (r >= 0 && S_ISREG(s.st_mode)) {
        return s.st_size;
    } else {
        return -1;
    }
}

int io300_seek(struct io300_file *f, off_t pos) {
    return fseek(f->f, pos, SEEK_SET);
}

int io300_readc(struct io300_file *f) {
    return fgetc(f->f);
}

int io300_writec(struct io300_file *f, int ch) {
    return fputc(ch, f->f);
}

ssize_t io300_read(struct io300_file *f, char *buff, size_t sz) {
    size_t const n = fread(buff, 1, sz, f->f);
    if (n != 0 || sz == 0 || !ferror(f->f)) {
        return n;
    } else {
        return -1;
    }
}

ssize_t io300_write(struct io300_file *f, const char *buff, size_t sz) {
    size_t const n = fwrite(buff, 1, sz, f->f);
    if (n != 0 || sz == 0 || !ferror(f->f)) {
        return n;
    } else {
        return -1;
    }
}

int io300_flush(struct io300_file *f) {
    return fflush(f->f);
}
