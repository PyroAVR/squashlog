#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    char *linebuf;
    size_t len;
    bool owned;
} buf_t;

typedef struct {
    size_t start, stop;
} buf_slice_t;

buf_t *buf_from_file(const char *restrict fname);
buf_t *buf_from_string_ref(char *data, size_t len);
buf_t *buf_from_string(char *data, size_t len);
buf_t *buf_advanced(buf_t *buf, size_t count);
size_t buf_slice_len(buf_slice_t *slice);
buf_t *buf_slice(buf_t *buf, buf_slice_t *slice);
void buf_free(buf_t *buf);
