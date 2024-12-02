#pragma once

#include <twig/tagbox.h>

#include <stddef.h>

#if 0
--- thinking

tl;dr of the issue: an argument may not be fully contained within the read
buffer, so we have to be able to perform dynamic-length buffering.

method 1: while(unpackarg(context) -> arg != NO_DATA) { use arg }
    this method requires context to contain a stream reader impl. and for the
    calling thread to either handle blocking if there is insufficient data, or
    be able to handle a null argument and re-try later when data is available.
    The stream reader would still need to hold any bytes not consumed.
    The context would also need to store information about whether it should
    try to unpack a new index or if it is continuing the decoding of a previous
    index

    - very easy to use in user code. A printf-relay could be done in ~10 lines of C
    - can still be non-blocking if the underlying FD is (no more complex than method 2)
    - buffering is performed by unpackarg... requires os, but we are targeting posix
    - status of decoding (no data vs. de-sync/decode error) is less easy to recover from by user

method 2: while(buf = read_data()) { arg = unpackarg(context, buf); if (arg) { use arg } else { accumulate more data }}
    this method places the burden of buffering on the caller and ensures
    non-blocking operation of unpackarg()

    - enables easy use of poll/select/epoll by user application
    - makes non-blocking / re-entrant behavior clear
    - code looks complex at a higher level

method 3: partial-arg decoding, eg. always consuming the full buffer.
    nigh impossible. what happens when the index is split across two read()
    calls? technically it could be done but the code will be a MESS for a tiny
    optimizaiton.

additionally: there isn't a sync byte. What does happen if we have a decode
error? If reliable transport is assumed, is method 1 the move?

just do method 1. simpler to write & understand. buffering is not that bad.
#endif

struct unpacker_ctx {
    size_t (*read)(void *impl, char *dest, size_t len);
    int (*next_arg_bytes)(void **ctx, void *spec);
    void *format_ctx;
    void *stream_ctx;
};

/**
 * Unpack one argument from a stream. Call with spec as the format string
 * on the first call
 */
struct tagbox unpackarg(struct unpacker_ctx *ctx, void *spec, size_t *skip);

/**
 * Unpack the index of a format string and declare how many bytes it consumed
 * bfmt: binary format of the host platform
 * buf: buffer to unpack from
 * skip: out, how many bytes were consumed by the index
 * return value: the index, in native endianness.
 */
uintmax_t unpack_idx(bfmt_t *bfmt, char *buf, size_t *skip);
