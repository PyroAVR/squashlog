#include "posix-stream.h"

#include "stream.h"

#include <stddef.h>
#include <stdio.h>

struct posix_stream *posix_stream_from_file(struct posix_stream *target, char *filename) {
    target->file = fopen(filename, "r");
    // no error handling...
    return target;
}

size_t stream_read(void *impl, char *dest, size_t len) {
    return fread(dest, len, 1, ((struct posix_stream*)impl)->file);
}
