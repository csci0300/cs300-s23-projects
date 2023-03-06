

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {

    int fd = open("onebyte.byte", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    char byte_all_ones = -1;
    ssize_t write_ret = write(fd, &byte_all_ones, 1);
    printf("after writing a byte of all ones (as an unsigned char: %x, as an int: %d), write returned %ld\n", (unsigned char)byte_all_ones, (int)byte_all_ones, write_ret);
    close(fd);


    FILE *f = fopen("onebyte.byte", "r");
    printf("the first byte read is %d\n", fgetc(f));
    printf("the second byte read is %d\n", fgetc(f));
    printf("the current value of ferror is %d\n", ferror(f));
    fclose(f);
    printf("after closing, the next byte is %d\n", fgetc(f));
    printf("now the value of ferror is %d\n", ferror(f));
    return 0;
}