#include <twig/streams/posix.h>

#include <twig/stream.h>

#include <stdio.h>
#include <string.h>

size_t stream_write(void *stream, char *buf, size_t len) {
    size_t idx = 0;
    posix_stream_t *s = stream;
    switch(s->tag) {
        case POSIX_STREAM_QUEUE:
            for(idx = 0; idx < len; idx++) {
                queue_push(s->queue, buf[idx]);
                if(QUEUE_FULL(s->queue)) {
                    break;
                }
            }
            return idx;

        case POSIX_STREAM_FILE:
            return fwrite(buf, 1, len, s->file);

        default:
            return 0;
    }

}
size_t stream_get_free(void *stream) {
    posix_stream_t *s = stream;
    switch(s->tag) {
        case POSIX_STREAM_QUEUE:
            return QUEUE_AVAIL(s->queue);

        case POSIX_STREAM_FILE:
            return (size_t)(-1);

        default:
            return 0;
    }
}
void stream_flush(void *stream, size_t len) {
    (void)len;
    posix_stream_t *s = stream;
    switch(s->tag) {
        case POSIX_STREAM_QUEUE:
            // no generic way to flush the queue, so this just returns.
        break;

        case POSIX_STREAM_FILE:
            fflush(s->file);
        break;
    }
}

posix_stream_t *posix_stream_from_file(posix_stream_t *stream, FILE *f) {
    stream->file = f;
    stream->tag = POSIX_STREAM_FILE;
    return stream;
}

posix_stream_t *posix_stream_from_queue(posix_stream_t *stream, queue_t *q) {
    stream->queue = q;
    stream->tag = POSIX_STREAM_QUEUE;
    return stream;
}
