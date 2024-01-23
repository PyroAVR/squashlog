#include "format_internal.h"

#include "formatter.h"

#include "platform/type_container.h"

#include <string_utils.h>

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <wchar.h>
#include <string.h>
#include <assert.h>

#define abs(a) (size_t)(((a) > 0) ? (a):-(a))

size_t sq_pack_idx(uintmax_t value, char *buffer) {
    int size = 9; // worst case result is 9 bytes.
    if((value & 0x7FULL) == value) {
        // value < 127
        size = 1;
        *buffer = value & 0x7F; // clear bit 7 to zero
    }
    else if((value & 0xFFULL) == value) {
        size = 2;
        buffer[0] = (char)0xCC; // 8-bit int format
        buffer[1] = value & 0xFF;
    }
    else if((value & 0xFFFFULL) == value) {
        size = 3;
        buffer[0] = (char)0xCD; // 16-bit int format
        buffer[1] = value & 0xFF;
        buffer[2] = (value & (0xFF << 8) >> 8);
    }
#if defined(OS_LOG_HUGE_FMTTBL) && OS_LOG_HUGE_FMTTBL == 1
    else if((value & 0xFFFFFFFFULL) == value) {
        size = 5;
        buffer[0] = 0xCE; // 32-bit int format
        buffer[1] = value & 0xFF;
        buffer[2] = (value & (0xFF << 8) >> 8);
        buffer[3] = (value & (0xFFUL << 16) >> 16);
        buffer[4] = (value & (0xFFUL << 24) >> 24);
    }
    else {
        buffer[0] = 0xCF; // 64-bit int format
        buffer[1] = value & 0xFF;
        buffer[2] = (value & (0xFF << 8) >> 8);
        buffer[3] = (value & (0xFFUL << 16) >> 16);
        buffer[4] = (value & (0xFFUL << 24) >> 24);
        buffer[5] = (value & (0xFFULL << 32) >> 32);
        buffer[6] = (value & (0xFFULL << 40) >> 40);
        buffer[7] = (value & (0xFFULL << 48) >> 48);
        buffer[8] = (value & (0xFFULL << 56) >> 56);
    }
#else
    else {
        size = 0; // return 0 size to indicate error
    }
#endif
    return size;
}

// TODO there is an edge case where the buffer fills up in the middle of a string
// copy. In this case, the next re-entry to this function will miss the remaining
// bytes of the string being packed.
// I think we can fix this by storing the offset from stpncpy in fmt_ctx_t, which
// will sadly consume another 2-4 bytes :(
//
// TODO stpncpy / wcpncpy do not do what we need. Implement it ourself.
// They zero out the whole buffer up to "size" and thus cause mayhem and slowdowns.

int sq_pack_r(fmt_ctx_t *ctx, void *spec, char *buf, size_t size, va_list ap) {
    int next_arg_bytes = ctx->fmt_func(spec, ctx->storage);
    size_t buf_idx = 0;
    // copy of va_arg value
    binfmt_box_t arg_copy;

    while(next_arg_bytes != 0) {
        // more data to be formatted
        if(next_arg_bytes > 0 && buf_idx + abs(next_arg_bytes) > size) {
            // exit early, no space in buffer
            break;
        }
        if(next_arg_bytes == -1) {
            // string specifier
            arg_copy.as_ptr[0] = strpcpy(buf + buf_idx, va_arg(ap, char *), size - buf_idx);
            // ensure bytes moved in buffer is correct for arg
            next_arg_bytes = ((char*)arg_copy.as_ptr[0]) - (buf + buf_idx);
        }
        else if(next_arg_bytes == -2) {
            assert("this case is broken");
            // wide string specifier
            arg_copy.as_ptr[0] = wcpncpy((wchar_t *)buf + buf_idx, (wchar_t *)va_arg(ap, wchar_t *), size - buf_idx);
            // ensure bytes moved in buffer is correct for arg
            next_arg_bytes = arg_copy.as_char - (buf + buf_idx);
        }
        // these next 3 cases handle the annoyance of integer type promotion,
        // which is required in order to pass an argument of the right size to
        // va_arg()
        else if(abs(next_arg_bytes) <= sizeof(int)) {
            arg_copy.as_smax[0] = va_arg(ap, int);
            memcpy(buf + buf_idx, &arg_copy.as_char, next_arg_bytes);
        }
        else if(abs(next_arg_bytes) <= sizeof(long)) {
            arg_copy.as_smax[0] = va_arg(ap, long);
            memcpy(buf + buf_idx, &arg_copy.as_char, next_arg_bytes);
        }
        else if(abs(next_arg_bytes) <= sizeof(long long)) {
            arg_copy.as_smax[0] = va_arg(ap, long long);
            memcpy(buf + buf_idx, &arg_copy.as_char, next_arg_bytes);
        }
        else {
            arg_copy.as_max_size_type = va_arg(ap, BINFMT_MAX_TYPE);
            memcpy(buf + buf_idx, &arg_copy.as_char, next_arg_bytes);
        }
        buf_idx += next_arg_bytes;
        if(buf_idx == size) {
            // buffer is full, exit early
            break;
        }
        next_arg_bytes = ctx->fmt_func(NULL, ctx->storage);
    }
    va_end(ap);
    return buf_idx;
}
