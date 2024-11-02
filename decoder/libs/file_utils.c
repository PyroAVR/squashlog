#include <file-utils.h>

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

buf_t *buf_from_file(const char *restrict fname) {
    buf_t *r = malloc(sizeof(buf_t));
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

buf_t *buf_from_string_ref(char *data, size_t len) {
    buf_t *r = malloc(sizeof(buf_t));
    if(!r) return NULL;
    r->linebuf = (char*)data;
    r->len = len;
    r->owned = false;
    return r;
}

buf_t *buf_from_string(char *data, size_t len) {
    buf_t *r = malloc(sizeof(buf_t));
    if(!r) return NULL;
    r->linebuf = data;
    r->len = len;
    r->owned = true; // take responsibility for freeing the buf
    return r;
}

buf_t *buf_advanced(buf_t *buf, size_t count) {
    buf_t *r = malloc(sizeof(buf_t));
    if(!r) return NULL;
    r->linebuf = buf->linebuf + count;
    r->len = buf->len - count;
    r->owned = false;
    return r;
}

size_t buf_slice_len(buf_slice_t *slice) {
    return slice->stop - slice->start;
}

buf_t *buf_slice(buf_t *buf, buf_slice_t *slice) {
    buf_t *r = malloc(sizeof(buf_t));
    if(!r) return NULL;
    r->linebuf = buf->linebuf + slice->start;
    r->len = slice->stop - slice->start;
    r->owned = false;
    return r;
}

void buf_free(buf_t *buf) {
    if(buf) {
        if(buf->linebuf && buf->owned) free(buf->linebuf);
        free(buf);
    }
}
