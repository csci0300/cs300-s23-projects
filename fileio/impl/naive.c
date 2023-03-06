#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../io300.h"


/*
    naive.c

    This implementation wraps the read/write syscalls and performs
    no userspace caching.
*/


struct io300_file {
    /* read,write,seek all take a file descriptor as a parameter */
    int fd;
};

struct io300_file *io300_open(const char *path, char *name) {
    struct io300_file *const ret = malloc(sizeof(*ret));
    if (ret) {
        ret->fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); // O_DIRECT?
        if (ret->fd == -1) {
            perror("open");
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
    free(f);
    return 0;
}

off_t io300_filesize(struct io300_file *f) {
    struct stat s;
    int const r = fstat(f->fd, &s);
    if (r >= 0 && S_ISREG(s.st_mode)) {
        return s.st_size;
    } else {
        return -1;
    }
}

int io300_seek(struct io300_file *f, off_t pos) {
    return lseek(f->fd, pos, SEEK_SET);
}

int io300_readc(struct io300_file *f) {
    unsigned char c;
    if (read(f->fd, &c, 1) == 1) {
        return c;
    } else {
        return -1;
    }
}
int io300_writec(struct io300_file *f, int ch) {
    char const c = (char)ch;
    return write(f->fd, &c, 1) == 1 ? ch : -1;
}

ssize_t io300_read(struct io300_file *f, char *buff, size_t sz) {
    return read(f->fd, buff, sz);
}
ssize_t io300_write(struct io300_file *f, const char *buff, size_t sz) {
    return write(f->fd, buff, sz);
}

int io300_flush(struct io300_file *f) {
    // This is a nop because we are not caching any data, so we can close
    // the file at any time without worrying.
    (void)f;
    return 0;
}