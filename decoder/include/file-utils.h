#pragma once

#include <stddef.h>
#include <stdbool.h>

struct fbuf {
    char *linebuf;
    size_t len;
    bool owned;
};

struct fbuf_slice {
    size_t start, stop;
};

struct fbuf *buf_from_file(const char *restrict fname);
struct fbuf *buf_from_string(char *data, size_t len, bool reference);
struct fbuf *buf_dup(struct fbuf *orig, bool reference);

struct fbuf *buf_advanced(struct fbuf *buf, size_t count);
size_t buf_slice_len(struct fbuf_slice *slice);
struct fbuf *buf_slice(struct fbuf *buf, struct fbuf_slice *slice);
void buf_free(struct fbuf *buf);
