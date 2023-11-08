#pragma once

#include "binfmt.h"

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <wchar.h>

#if CHAR_BIT != 8
#error "decoder code assumes CHAR_BIT == 8."
#endif

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

#if !defined BINFMT_MAX_TYPE
#define BINFMT_MAX_TYPE long double
#endif

/**
 * Data type to represent an integer-type from a host device in native data
 * types.
 *
 * data: union of all types that are understood by the decoder.
 * tag: data type tag, specifies what size of value is contained in data.
 */
typedef struct {
    union {
        int8_t as_s8[sizeof(BINFMT_MAX_TYPE) / sizeof(int8_t)];
        int16_t as_s16[sizeof(BINFMT_MAX_TYPE) / sizeof(int16_t)];
        int32_t as_s32[sizeof(BINFMT_MAX_TYPE) / sizeof(int32_t)];
        int64_t as_s64[sizeof(BINFMT_MAX_TYPE) / sizeof(int64_t)];

        uint8_t as_u8[sizeof(BINFMT_MAX_TYPE) / sizeof(uint8_t)];
        uint16_t as_u16[sizeof(BINFMT_MAX_TYPE) / sizeof(uint16_t)];
        uint32_t as_u32[sizeof(BINFMT_MAX_TYPE) / sizeof(uint32_t)];
        uint64_t as_u64[sizeof(BINFMT_MAX_TYPE) / sizeof(uint64_t)];

        float as_float[sizeof(BINFMT_MAX_TYPE) / sizeof(float)];
        double as_double[sizeof(BINFMT_MAX_TYPE) / sizeof(double)];
        long double as_long_double[sizeof(BINFMT_MAX_TYPE) / sizeof(long double)];
    } data;
    binfmt_tag_t tag;
} binfmt_data_t;

/**
 * Load binfmt file from a path and check for version compatibility.
 * The endianness of size fields are corrected for the decoder machine.
 */
binfmt_t *load_binfmt_from_file(char *path);

/**
 * Functions yielding decoder (decoding machine) types from host (encoding
 * machine) types.
 * It is assumed that the integer sizes of the decoding machine are greater than
 * or equal to that of the encoding machine.
 * That is to say, decoding information from an x86_64 computer on an AVR will
 * not work, since the return types of these functions on AVR will not fit
 * their x86_64 counterparts.
 *
 * An explanation of why per-type decoders are necessary:
 * Some platforms (such as blackfin / SHARC / c2k) have CHAR_BIT > 8
 * where less-than-char-bit sized integers will effectively be padded up to
 * one char unit. Eg. on SHARC, CHAR_BIT = 32.
 *
 * If the decoder is running on a big-endian system, a 16-bit value from SHARC
 * needs to be byte-swapped like so: 4321 -> 12. 8-bit-bytes 4 and 3 are all 0.
 * memcpy() on the host platform will "do the right thing", and respect CHAR_BIT
 * by copying the correct number of *bits* to represent the actual *bytes* on
 * the host architecture. Eg. the encoder will send 4 bytes for a char on SHARC,
 * but only 1 byte on x86_64.
 *
 * The tl;dr version is that we have to correct for endianness *and* CHAR_BIT
 * between the host architecture and the decoder architecture.
 */

/**
 * Byte-swapping functions, operating in decoder endianness
 */
uint64_t binfmt_byte_order_adj64(uint64_t src);
uint32_t binfmt_byte_order_adj32(uint32_t src);
uint16_t binfmt_byte_order_adj16(uint16_t src);

/**
 * CHAR_BIT and byteswap-correcting functions. These deal with differences in
 * CHAR_BIT (and endianness for actual data < sizeof(char)) first, and then byte
 * swap to decoder endianness
 */
// TODO
// AVR has sizeof(wint_t) == 4, x86_64 has sizeof(wint_t) == 2... this breaks
// the above assumption that decoder system has wider types than host system.
// wat?? why would avr need that big of wint...
// changing everything to uintmax_t works so long as it is > the max size in
// binfmt header.
//
// decoder cannot then generically convert to appropriate runtime sizes, so
// we're stuck with uintmax_t
//
// functions using that data will need to know whether to re-interp as signed,
// and what the actual size should be, uintmax_t is just the container.
// the format string is the link containing the actual data size.
// could we return a more complex type, eg. a uintmax_t, byte mask, and sign?

uint64_t binfmt_decode_i64(binfmt_t *fmt, char *src_bytes);
uint32_t binfmt_decode_i32(binfmt_t *fmt, char *src_bytes);
uint16_t binfmt_decode_i16(binfmt_t *fmt, char *src_bytes);
uint8_t binfmt_decode_i8(binfmt_t *fmt, char *src_bytes);
long binfmt_decode_long(binfmt_t *fmt, char *src_bytes);
int binfmt_decode_int(binfmt_t *fmt, char *src_bytes);

size_t binfmt_decode_size_t(binfmt_t *fmt, char *src_bytes);
wint_t binfmt_decode_wchar_t(binfmt_t *fmt, char *src_bytes);

long double binfmt_decode_long_double(binfmt_t *fmt, char *src_bytes);
double binfmt_decode_double(binfmt_t *fmt, char *src_bytes);
float binfmt_decode_float(binfmt_t *fmt, char *src_bytes);


uintptr_t binfmt_decode_ptr(binfmt_t *fmt, char *src_bytes);
ptrdiff_t binfmt_decode_ptrdiff(binfmt_t *fmt, char *src_bytes);
