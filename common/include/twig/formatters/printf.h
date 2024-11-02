#pragma once

/**
 * Parse spec, returning the number of bytes required to hold the next format
 * argument within spec. Initializes the string context in ctx if spec is
 * non-NULL. Pass NULL for spec to continue parsing the most recent spec string.
 * spec: format-spec string in printf style
 * returns: number of bytes required for next arg.
 */
int printf_next_arg_bytes(const char **ctx, const char *spec);

// TODO internal, but putting this here for now because I don't want to make
// a private header shared between encoder and decoder
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
    PTR_CONV_FLAG = 1 << 12
};

