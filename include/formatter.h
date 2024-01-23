#pragma once

#include <stddef.h>

/**
 * Formatter: convert an implementation-specific notation for an arrangement of
 * data into the squashlog message format.
 */

/**
 * Generic type of a format function. To be called by common formatter functions
 * in a manner similar to strtok_r, where *spec is valid on the first call and
 * is NULL in subsequent calls. Memory will be provided at *ctx for context
 * storage and shall be invalidated / reset by the format function when a
 * non-null argument is provided to spec.
 *
 * spec: implementation-defined data format specification.
 * ctx: implementation-defined context variable.
 * returns: number of bytes which shall be copied from the next argument to the
 *          format function.
 *
 *          special return values:
 *           0: No further bytes to pack
 *          -1: value to pack is a null-terminated character string
 *          -2: value to pack is a null-terminated wide-character string
 */
typedef int (fmt_next_size)(void *spec, void *ctx);

// this absorbs at most 20 bytes... if that's too tight, you probably shouldn't
// be using printf debugging... just saying.
// NOTE: storage needs to point to formatter-implementation-specific data.
typedef struct {
    fmt_next_size *fmt_func;
    void *storage;
    size_t strcpy_offset;
} fmt_ctx_t;
