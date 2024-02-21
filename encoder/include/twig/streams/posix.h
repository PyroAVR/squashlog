#pragma once
#include <twig/stream.h>

#include <twig/streams/byte_queue.h>

#include <stddef.h>
#include <stdio.h>

typedef struct {
    union {
        queue_t *queue;
        FILE *file;
    };
    enum {
        POSIX_STREAM_QUEUE,
        POSIX_STREAM_FILE,
    } tag;
} posix_stream_t;

posix_stream_t *posix_stream_from_file(posix_stream_t *stream, FILE *f);
posix_stream_t *posix_stream_from_queue(posix_stream_t *stream, queue_t *q);
