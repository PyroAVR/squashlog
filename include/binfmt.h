#pragma once
#include <stdint.h>

#define BINFMT_ENDIANNESS_DETECT32 0x01020304U
#define BINFMT_ENDIANNESS_DETECT16 0x0102U
// "BFMT"
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define BINFMT_MAGIC 0x42464d54
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define BINFMT_MAGIC 0x544d4642
#elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
// __ORDER_CHAOS_ENDIAN__
#define BINFMT_MAGIC 0x4d544246
#endif

// bytes: major, minor
#define BINFMT_VERSION 0x0001U


#if !defined(BINFMT_U32_TYPE)
#define BINFMT_U32_TYPE uint32_t
#endif

#define u32 BINFMT_U32_TYPE

/**
 * All fields here are 32-bit - won't be included in the actual firmware binary,
 * size is not a concern.
 * "magic" must be packed to 8-bit bytes, that is, on platforms where CHAR_BIT != 8, the binfmt struct must be filled such that when the struct is read on a system
 * where CHAR_BIT == 8, there are no gaps.
 */
typedef struct {
    u32 magic;
    u32 version;
    u32 byte_order_32;
    u32 byte_order_16;
    u32 char_bit;
    u32 double_size; // %f, g
    u32 long_double_size; // %lf, lg
    u32 float_size;
    u32 size_t_size;
    u32 wint_t_size;
    u32 pointer_size;
    u32 ptrdiff_size;
    u32 long_size;
    u32 int_size;
} binfmt_t;
