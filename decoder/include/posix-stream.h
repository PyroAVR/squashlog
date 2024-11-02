#pragma once

#include <stdio.h>

// TODO a commonized posix stream could involve abstraction for epoll / select
// and normal read/write + opening from various sources, eg. sockets, ttys,
// whatever. Could also just be different streams for each of those.
struct posix_stream {
    FILE *file;
};

struct posix_stream *posix_stream_from_file(struct posix_stream *target, char *filename);
