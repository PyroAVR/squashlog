#include <unix_bytestream.h>

#include <bytestream.h>

#include <stdio.h>
#include <string.h>

void unix_buf_init(void) {}

int buf_write(void *buf, char *str, size_t len) {
    (void)buf;
    return fwrite(str, 1, len, stdout);
}

// return count of free bytes
size_t buf_get_free(void *buf) {
    (void)buf;
    return (size_t)(-1);
}

void buf_flush(void *buf, size_t until_free) {
    // this does nothing on stdio streams, no need to exercise control over them
    (void)buf;
    (void)until_free;
}
