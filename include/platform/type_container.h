#pragma once

#include "platform/binformat.h"

#include <stdint.h>

/**
 * Datatype widths understood by binfmt
 */
typedef enum {
    BINFMT_SIGNED = (1 << 7),
    BINFMT_I8 = 1,
    BINFMT_I16 = 2,
    BINFMT_I32 = 3,
    BINFMT_I64 = 4,
    BINFMT_F32 = 5, // float
    BINFMT_F64 = 6, // double
    BINFMT_F80 = 7, // long double
} binfmt_tag_t;

/**
 * A universal box. If it fits, it ships.
 * This is a union of all the types understood by squashlog
 */
typedef union {
        char as_char[sizeof(BINFMT_MAX_TYPE) / sizeof(char)];
        int as_int[sizeof(BINFMT_MAX_TYPE) / sizeof(int)];
#if defined(BINFMT_HAVE_STD8)
        int8_t as_s8[sizeof(BINFMT_MAX_TYPE) / sizeof(int8_t)];
#endif
        int16_t as_s16[sizeof(BINFMT_MAX_TYPE) / sizeof(int16_t)];
        int32_t as_s32[sizeof(BINFMT_MAX_TYPE) / sizeof(int32_t)];
        int64_t as_s64[sizeof(BINFMT_MAX_TYPE) / sizeof(int64_t)];
        uintmax_t as_smax[sizeof(BINFMT_MAX_TYPE) / sizeof(intmax_t)];

#if defined(BINFMT_HAVE_STD8)
        uint8_t as_u8[sizeof(BINFMT_MAX_TYPE) / sizeof(uint8_t)];
#endif
        uint16_t as_u16[sizeof(BINFMT_MAX_TYPE) / sizeof(uint16_t)];
        uint32_t as_u32[sizeof(BINFMT_MAX_TYPE) / sizeof(uint32_t)];
        uint64_t as_u64[sizeof(BINFMT_MAX_TYPE) / sizeof(uint64_t)];
        uintmax_t as_umax[sizeof(BINFMT_MAX_TYPE) / sizeof(uintmax_t)];

        void *as_ptr[sizeof(BINFMT_MAX_TYPE) / sizeof(void *)];

        float as_float[sizeof(BINFMT_MAX_TYPE) / sizeof(float)];
        double as_double[sizeof(BINFMT_MAX_TYPE) / sizeof(double)];
        long double as_long_double[sizeof(BINFMT_MAX_TYPE) / sizeof(long double)];
        BINFMT_MAX_TYPE as_max_size_type;
} binfmt_box_t;

/**
 * Data type to represent an integer-type from a host device in native data
 * types.
 *
 * data: union of all types that are understood by the decoder.
 * tag: data type tag, specifies what size of value is contained in data.
 */
typedef struct {
    binfmt_box_t data;
    binfmt_tag_t tag;
} binfmt_data_t;
