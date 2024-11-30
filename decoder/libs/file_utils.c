#include <file-utils.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

struct fbuf *buf_from_file(const char *restrict fname) {
    struct fbuf *r = malloc(sizeof(struct fbuf));
    if(!r) return NULL;

    FILE *f = fopen(fname, "r");
    int start = ftell(f);
    fseek(f, 0, SEEK_END);
    int stop = ftell(f);
    rewind(f);
    size_t len = stop - start;
    r->linebuf = malloc(sizeof(char)*len);
    if(!r->linebuf) {
        free(r);
        return NULL;
    }
    fread((void*)r->linebuf, sizeof(char), len, f);
    fclose(f);
    r->len = len;
    r->owned = true;
    return r;
}

struct fbuf *buf_from_string(char *data, size_t len, bool reference) {
    struct fbuf *r = malloc(sizeof(struct fbuf));
    if(!r) return NULL;
    r->linebuf = data;
    r->len = len;
    r->owned = reference; // take responsibility for freeing the buf
    return r;
}

struct fbuf *buf_advanced(struct fbuf *buf, size_t count) {
    struct fbuf *r = malloc(sizeof(struct fbuf));
    if(!r) return NULL;
    r->linebuf = buf->linebuf + count;
    r->len = buf->len - count;
    r->owned = false;
    return r;
}

size_t buf_slice_len(struct fbuf_slice *slice) {
    return slice->stop - slice->start;
}

struct fbuf *buf_slice(struct fbuf *buf, struct fbuf_slice *slice) {
    struct fbuf *r = malloc(sizeof(struct fbuf));
    if(!r) return NULL;
    r->linebuf = buf->linebuf + slice->start;
    r->len = slice->stop - slice->start;
    r->owned = false;
    return r;
}

void buf_free(struct fbuf *buf) {
    if(buf) {
        if(buf->linebuf && buf->owned) free(buf->linebuf);
        free(buf);
    }
}
