#pragma once

#include <twig/tagbox.h>

#include <stddef.h>

struct unpacker_ctx {
    size_t (*read)(void *impl, char *dest, size_t len);
    int (*next_arg_bytes)(void **ctx, void *spec);
    void *format_ctx;
    void *stream_ctx;
};

struct tagbox unpackarg(struct unpacker_ctx *ctx, void *spec, char *buf, size_t len);

/**
 * Unpack the index of a format string and declare how many bytes it consumed
 * bfmt: binary format of the host platform
 * buf: buffer to unpack from
 * skip: out, how many bytes were consumed by the index
 * return value: the index, in native endianness.
 */
uintmax_t unpack_idx(bfmt_t *bfmt, char *buf, size_t *skip);
