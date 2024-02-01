#pragma once
// TODO this file should specify the interface for a bytestream, which allows writes to an internal buffer.
// initialization will be left up to the implementation (eg. none on linux, just use putc/puts to dump the buffer. on embedded, perhaps uart_tx or similar)

#include <stddef.h>

// write bytes to buffer
int buf_write(void *buf, char *str, size_t len);

// return count of free bytes
size_t buf_get_free(void *buf);

// block until the buffer has at least until_free bytes available
void buf_flush(void *buf, size_t until_free);
