#pragma once

#include <twig/bfmt.h>

#include <stdint.h>

/**
 * A universal box. If it fits, it ships.
 * This is a union of all the types understood by squashlog
 */
typedef union {
        char as_char[sizeof(BFMT_MAX_TYPE) / sizeof(char)];
        int as_int[sizeof(BFMT_MAX_TYPE) / sizeof(int)];
#if defined(BFMT_HAVE_STD8)
        int8_t as_s8[sizeof(BFMT_MAX_TYPE) / sizeof(int8_t)];
#endif
        int16_t as_s16[sizeof(BFMT_MAX_TYPE) / sizeof(int16_t)];
        int32_t as_s32[sizeof(BFMT_MAX_TYPE) / sizeof(int32_t)];
        int64_t as_s64[sizeof(BFMT_MAX_TYPE) / sizeof(int64_t)];
        uintmax_t as_smax[sizeof(BFMT_MAX_TYPE) / sizeof(intmax_t)];

#if defined(BFMT_HAVE_STD8)
        uint8_t as_u8[sizeof(BFMT_MAX_TYPE) / sizeof(uint8_t)];
#endif
        uint16_t as_u16[sizeof(BFMT_MAX_TYPE) / sizeof(uint16_t)];
        uint32_t as_u32[sizeof(BFMT_MAX_TYPE) / sizeof(uint32_t)];
        uint64_t as_u64[sizeof(BFMT_MAX_TYPE) / sizeof(uint64_t)];
        uintmax_t as_umax[sizeof(BFMT_MAX_TYPE) / sizeof(uintmax_t)];

        void *as_ptr[sizeof(BFMT_MAX_TYPE) / sizeof(void *)];

        float as_float[sizeof(BFMT_MAX_TYPE) / sizeof(float)];
        double as_double[sizeof(BFMT_MAX_TYPE) / sizeof(double)];
        long double as_long_double[sizeof(BFMT_MAX_TYPE) / sizeof(long double)];
        BFMT_MAX_TYPE as_max_size_type;
} box_t;
