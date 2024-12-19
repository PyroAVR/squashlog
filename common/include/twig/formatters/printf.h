#pragma once

#include <stddef.h>

/**
 * Read a string until a '%' is found.
 * Initializes the string context in ctx if spec is non-NULL.
 * Pass NULL for spec to continue parsing the most recent spec string.
 * spec: format-spec string in printf style
 * returns: pointer to first '%' in spec, or NULL if not found.
 */
const char *printf_skip_non_spec(const char *spec);

/**
 * Symbols declaring the meaning of each flag in the printf_spec_flags
 * return value
 */
enum {
    BYTE_MOD_FLAG = 1 << 0,
    SHORT_MOD_FLAG = 1 << 1,
    LONG_MOD_FLAG = 1 << 2,
    LONG_LONG_MOD_FLAG = 1 << 3,
    LONG_DOUBLE_MOD_FLAG = 1 << 4,
    INTMAX_MOD_FLAG = 1 << 5,
    SIZE_T_MOD_FLAG = 1 << 6,
    PTRDIFF_T_MOD_FLAG = 1 << 7,
    INT_CONV_FLAG = 1 << 8,
    DOUBLE_CONV_FLAG = 1 << 9,
    CHAR_CONV_FLAG = 1 << 10,
    STR_CONV_FLAG = 1 << 11,
    PTR_CONV_FLAG = 1 << 12,
    UNSIGNED_FLAG = 1 << 13,
};

/**
 * Get a set of flags from a printf-style format specifier which correlate to
 * the number of bytes needed to represent the coded type on any platform.
 * The return value of this function may be used with printf_spec_bytes to
 * get the actual number of bytes.
 * Example:
 *     On x86_64, %ld correlates to long int and consumes 8 bytes.
 *     On armv7, %ld consumes 4 bytes.
 *     This function returns 0x12 (integer, long modifier) on both platforms.
 * spec: format string starting with %
 * skip: out, length in characters of spec consumed by parsing
 * return value: flags, for use with printf_spec_bytes
 */
int printf_spec_flags(const char *spec, size_t *skip);
