#pragma once
#include <stdint.h>

#define BINFMT_ENDIANNESS_DETECT32 0x01020304
#define BINFMT_ENDIANNESS_DETECT16 0x0102

/**
 * All fields here are 32-bit - won't be included in the actual firmware binary,
 * size is not a concern.
 */
typedef struct {
    uint32_t byte_order_32;
    uint16_t byte_order_16;
    uint32_t char_bit;
    uint32_t double_size; // %f, g
    uint32_t long_double_size; // %lf, lg
    uint32_t float_size;
    uint32_t size_t_size;
    uint32_t wint_t_size;
    uint32_t pointer_size;
    uint32_t ptrdiff_size;
    uint32_t long_size;
    uint32_t int_size;
} binfmt_t;
