#pragma once
#include "formatter.h"

#include "platform/binformat.h"

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

/**
 * Utility functions needed for implementations of a squashlog formatter
 */


/**
 * Pack integer-typed index of format identifier into msgpack int-family format.
 */
size_t sq_pack_idx(uintmax_t value, char *buffer);

/**
 * Pack generically typed data into a message buffer
 *
 * Note: this function is reentrant and calls a reentrant function:
 * ctx->fmt_func. If the buffer is full (return value == size), this function
 * needs to be called again after flushing the buffer, but with spec = NULL.
 */
int sq_pack_r(fmt_ctx_t *ctx, void *spec, char *buf, size_t size, va_list ap);
