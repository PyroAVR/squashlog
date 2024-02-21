#pragma once
/**
 * Minimum interface definition for a stream - something which consumes bytes
 * and processes them without knowledge of the interpretation of the bytes being
 * sent. A data link.
 *
 * At a minimum, stream implementations must implement the functions in this
 * header, in addition to their own implementation-defined initialization.
 */

#include <stddef.h>

/**
 * Write a sequence of CHAR_BIT sized entities to the stream.
 * If len > stream_get_free(impl), the behavior is IMPLEMENTATION DEFINED.
 * If str is NULL or len == 0, no bytes shall be written, 0 shall be returned.
 * Returns the number of CHAR_BIT sized entities written.
 */
size_t stream_write(void *impl, char *str, size_t len);

/**
 * Return how many chars are available before blocking will occur.
 */
size_t stream_get_free(void *impl);

/**
 * TODO the packer is not able to use this at the moment, since there is neither
 * OS awareness nor blocking "wait for flush" logic. remove?
 * Cause any buffered data in the stream implementation to be handled
 * immediately. When until_free is > 0, block until the number of available
 * chars is at least until_free. Otherwise, send only the bytes which can be
 * sent without blocking.
 */
void stream_flush(void *impl, size_t until_free);
