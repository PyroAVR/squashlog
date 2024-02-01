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
 * ctx: implementation-defined context variable.
 * spec: implementation-defined data format specification.
 * returns: number of bytes which shall be copied from the next argument to the
 *          format function.
 *
 *          special return values:
 *           0: No further bytes to pack
 *          -1: value to pack is a null-terminated character string
 *          -2: value to pack is a null-terminated wide-character string
 */
typedef int (arg_size_f)(void *ctx, void *spec);

typedef struct {
    arg_size_f *next_arg_bytes;
    void *buffer; // buffer implementation
    void *format_ctx; // format string implementation context argument
} fmt_ctx_t;
