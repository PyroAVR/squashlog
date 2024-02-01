#pragma once

#include "utils.h"

#include <stdint.h>
#include <stddef.h>

// Place the indices of the fmtstrs in their own section, for extraction during
// decoding.
#if !defined(OS_LOG_OFFSETS_SECTION)
#define OS_LOG_OFFSETS_SECTION ".os_log_fmtstrs"
#endif

// TODO place actual string data in its own section

#if !defined(OS_LOG_HUGE_FMTTBL)
#define OS_LOG_HUGE_FMTTBL 0
#endif

#if OS_LOG_HUGE_FMTTBL == 1
#define OS_LOG_IDX_SZ_MAX 9
#else
#define OS_LOG_IDX_SZ_MAX 3
#endif

// create zeroth entry in the format string section, denoting the beginning
extern SECTION(OS_LOG_OFFSETS_SECTION) const char *_os_log_fmtstrs_start;

// since the offset value is an immediate / macro pasted value, even a stripped
// binary will have the correct offsets.
#define OS_LOG_FMT_NAME(str, out_id) {\
    static const char \
    *PASTE(_OS_LOG_, __LINE__) SECTION(OS_LOG_OFFSETS_SECTION) = str;\
    out_id = (uintptr_t)(&PASTE(_OS_LOG_, __LINE__) - &_os_log_fmtstrs_start);\
}

#define os_log(fmt, ...) {\
    int offset;\
    OS_LOG_FMT_NAME(fmt, offset);\
    printf("%i: " fmt, offset, __VA_ARGS__);\
}

#define os_log_s(buf, len, fmt, ...) {\
    int offset;\
    OS_LOG_FMT_NAME(fmt, offset);\
    _os_log_snprintf(buf, len, offset, fmt, __VA_ARGS__);\
}

/**
 * Pack an integer representing the format string index into the smallest number
 * of bytes which can represent it.
 * The msgpack int format family is used for this task, but only the unsigned
 * integers are supported. Additionally, the big-endian ordering of msgpack is
 * not honored. Do not use this function for generic msgpack, do not decode the
 * bytes returned from this function using any msgpack implementation.
 * value: integer value to pack
 * buffer: buffer to pack into. Size should be 9 bytes.
 * returns: #(8-bit bytes) of msgpack-encoded type and integer value, 0 on error
 */
int os_log_pack_idx(uintmax_t value, char *buffer);

/**
 * Like snprintf, but outputs an os_log string.
 * buffer: pointer to byte buffer where the log message will be packed
 * size: length of buffer. Log message will be truncated to this number.
 * ...: varargs for fmt, printf format
 * returns: number of bytes actually packed
 */
int _os_log_snprintf(char *buffer, size_t size, int offset, char *fmt, ...);
