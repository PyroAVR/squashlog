#pragma once
#include <twig/stream.h>

#include <twig/streams/byte_queue.h>
#include <twig/packer.h>

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

static inline packer_ctx_t *posix_stream_init(packer_ctx_t *ctx, posix_stream_t *stream) {
    ctx->stream = stream;
    return ctx;
}

posix_stream_t *posix_stream_from_file(posix_stream_t *stream, FILE *f);
posix_stream_t *posix_stream_from_queue(posix_stream_t *stream, queue_t *q);
