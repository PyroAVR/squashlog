#include <twig/unpacker.h>

#include <twig/bfmt.h>
#include <twig/tagbox.h>
#include <twig/strtab.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> // FIXME better debug logging

uintmax_t unpack_idx(bfmt_t *bfmt, char *buf, size_t *skip) {
    // msgpack format w/o big-endian swapping
    uintmax_t r = 0;
    unsigned int size = 0;
    if((buf[0] & 0x7FULL) == buf[0]) {
        // 7 bit
        r = buf[0];
        return r;
    }
    else {
        size = (unsigned int)buf[0] - 0xCBU;
    }
    memcpy(&r, buf + 1, size & sizeof(uintmax_t)); // mask to remove warning despite bounded math above
    if(skip != NULL) {
        *skip = size;
    }

    switch(size) {
        case 0:
        case 1:
            // 7 or 8-bit int
            break;
        case 2:
            // 16-bit int
            if(bfmt->byte_order_16 == 0x0102U) {
                // big
                be16toh(r);
            }
            else {
                // little
                le16toh(r);
            }
            break;
        case 0xCEU:
            // 32-bit int
            if(bfmt->byte_order_32 == 0x01020304U) {
                // big
                r = be32toh(r);
            }
            else if(bfmt->byte_order_32 == 0x04030201U) {
                // little
                r = le32toh(r);
            }
            else {
                // order chaos endian
                // TODO if host system has bigger ints than decoder, gg
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
                r = be16toh(((uint16_t*)r)[0]) | be16toh(((uint16_t*)r)[1]);
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
                r = (be16toh(((uint16_t*)r)[0]) >> 16) | (be16toh(((uint16_t*)r)[1]) << 16);
#else
                // I feel bad for you, man.
#endif
            }
            break;
        case 0xCFU:
            // 64-bit int
            // TODO endianness detector does not cover this case, assume
            // order is the same...
            break;
    }
    return r;
}

struct tagbox unpackarg(struct unpacker_ctx *ctx) {
    struct tagbox r = {.tag = NO_DATA, .str = NULL};
    char buf[256]; // local buffer to read into TODO magic number
    size_t bytes_available = ctx->read(ctx->stream_ctx, buf, 256);
    size_t bytes_needed = 0;
    size_t skip = 0;
    if(ctx->new_spec) {
        uintmax_t idx = unpack_idx(ctx->host_bfmt, buf, &skip);
        if(idx == (uintmax_t)-1) {
            fprintf(stderr, "big huge error, could not unpack index\n");
            return r;
        }
        ctx->curr_spec = strtab_lookup(ctx->host_strtab, idx);
        if(ctx->curr_spec == NULL) {
            fprintf(stderr, "lookup for index %lu failed\n", idx);
            return r;
        }
        bytes_needed = ctx->next_arg_bytes(ctx->format_ctx, ctx->curr_spec);
        bytes_available -= skip;
    }
    else {
        bytes_needed = ctx->next_arg_bytes(ctx->format_ctx, NULL);
    }
    // process arg
    if(bytes_needed > bytes_available) {
        // gg
        fprintf(stderr, "not enough data available, bailing out. Needed: %lu, Available: %lu\n", bytes_needed, bytes_available);
        return r;
    }
    // TODO how do I get the encoded type? need it for signed vs. unsigned
    // modify printf_flags to also yield signedness flags, but only on decoder
    // builds.

    // REMAINING ALGORITHM STEPS:
    // - read bytes
    // - create functions to process ints, floats, and strings.
    // REMAINING WORK:
    // - make this loop in a way where the buffer is continually re-filled, but
    // not overwritten when called twice w/o consuming the entire buffer
    // - also fix error handling / printing / exit codes
    return r;
}
