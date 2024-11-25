#include <twig/unpacker.h>

#include <twig/bfmt.h>
#include <twig/tagbox.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
    memcpy(&r, buf + 1, size);
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

// return ONE (next) arg value as a tagbox.
// call again with NULL spec to continue consuming args
struct tagbox unpackarg(struct unpacker_ctx *ctx, void *spec, char *buf, size_t len) {
    struct tagbox r;
    r.tag = AS_U16;
    r.data.as_u16[0] = 32768;
    // note next_arg_bytes works just like encoder, pass NULL on all but first call
    // set current_bytes = 0
    // if x = next_arg_bytes(ctx, bfmt, spec, buf) returns > 0:
    //     if current_bytes + x >= len: break
    //     read x bytes from buf
    //     current_bytes += x
    //     ... how to get the value out? return box with tag for type?
    //     no convenient va_list here, could just use alibc_array since it's
    //     already included.
    return r;
}
