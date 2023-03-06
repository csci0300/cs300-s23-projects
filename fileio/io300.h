#ifndef IO300_H
#define IO300_H

#include <sys/types.h>


/*
    The exact contents of the io300_file struct are not yet
    specified, that is up to you to define for your implementation.
*/
struct io300_file;


/*
    Allocate and initialize a new io300_file struct that will wrap the given
    file path. In addition, give the file a description for debugging purposes.
*/
struct io300_file *io300_open(const char *path, char *description);

/*
    Close and cleanup the given io300_file. Remember that when a file is
    closed, you need to write any cached data that resides in RAM to disk.
    This probably means calling flush().

    Return 0 on success, -1 on failure.
*/
int io300_close(struct io300_file *f);


/*
    Return the number of bytes in the file or -1 if this
    computation is not possible.
*/
off_t io300_filesize(struct io300_file *f);

/*
    seek() repositions the file offset to the given value. This causes
    subsequent reads and writes to take place from the new offset.

    Upon successful completion, seek() returns the resulting offset location
    as measured in bytes from the beginning  of the file.  On error, the
    value (off_t)-1 is returned.
*/
int io300_seek(struct io300_file *f, off_t pos);

/*
    readc() reads a single byte from the file and returns it.
    Return -1 on failure or if the end of the file has been reached.
*/
int io300_readc(struct io300_file *f);

/*
    writec() writes a single byte to the file.
    Return the byte that was written upon success and -1 on failure.
*/
int io300_writec(struct io300_file *f, int ch);


/*
    Read `nbytes` from the file into `buff`. Assume that the buffer is large enough.
    On failure, return -1. On success, return the number of bytes that were
    placed into the provided buffer.
*/
ssize_t io300_read(struct io300_file *f, char *buff, size_t nbytes);

/*
    Write `nbytes` from the start of `buff` into the file. Assume that the buffer
    is large enough.
    On failure, return -1. On success, return the number of bytes that were
    written to the file.
*/
ssize_t io300_write(struct io300_file *f, const char *buff, size_t nbytes);


/*
    Flush any in-RAM data (caches) to disk.
*/
int io300_flush(struct io300_file *f);

#endif
