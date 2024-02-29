#include <twig/packer.h>

#include <twig/stream.h>
#include <twig/bfmt.h>
#include <twig/box.h>

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <wchar.h>
#include <string.h>

#define abs(a) (size_t)(((a) > 0) ? (a):-(a))

size_t pack_idx(packer_ctx_t *ctx, uintmax_t value) {
    int size = 9; // worst case result is 9 bytes.
    unsigned char type_flag = 0;
    if((value & 0x7FULL) == value) {
        // value < 127
        size = 1;
    }
    else if((value & 0xFFULL) == value) {
        size = 2;
        type_flag = 0xCC; // 8-bit int format
    }
    else if((value & 0xFFFFULL) == value) {
        size = 3;
        type_flag = 0xCD; // 16-bit int format
    }
#if defined(OS_LOG_HUGE_FMTTBL) && OS_LOG_HUGE_FMTTBL == 1
    else if((value & 0xFFFFFFFFULL) == value) {
        size = 5;
        type_flag = 0xCE; // 32-bit int format
    }
    else {
        type_flag = 0xCF; // 64-bit int format
    }
#else
    else {
        size = 0; // return 0 size to indicate error
    }
#endif
    if(size > 1) {
        stream_write(ctx->stream, (char*)&type_flag, 1);
        size--;
    }
    if(size) {
        stream_write(ctx->stream, (char*)&value, size);
    }
    return size;
}

int vpackargs(packer_ctx_t *ctx, void *spec, va_list ap) {
    int args_processed = 0;
    size_t bytes_avail = stream_get_free(ctx->stream);
    // further refinement of this value follows
    int bytes_needed = ctx->next_arg_bytes(ctx->formatter, spec);
    // copy of va_arg value
    box_t arg;

    // while there are remaining args to format
    while(bytes_needed != 0) {
        if(bytes_needed == -1) {
            // string specifier
            arg.as_ptr[0] = va_arg(ap, char *);
            bytes_needed = strnlen(arg.as_ptr[0], bytes_avail);
            if((size_t)bytes_needed < bytes_avail) {
                bytes_avail -= stream_write(ctx->stream, arg.as_ptr[0], bytes_needed);
            }
        }
        else if(bytes_needed == -2) {
            // w i d e  string specifier
            arg.as_ptr[0] = va_arg(ap, char *);
            bytes_needed = wcsnlen(arg.as_ptr[0], bytes_avail) * sizeof(wchar_t);
            if((size_t)bytes_needed <= bytes_avail) {
                bytes_avail -= stream_write(ctx->stream, arg.as_ptr[0], bytes_needed);
            }
        }
        else if((size_t)bytes_needed < bytes_avail) {
            // this handles the annoyance of integer type promotion, which is
            // required in order to pass an argument of the right size to
            // va_arg()
            switch(abs(bytes_needed)) {
                case sizeof(int):
                    arg.as_smax[0] = va_arg(ap, int);
                break;

                case sizeof(long):
                    arg.as_smax[0] = va_arg(ap, long);
                break;

// TODO these macros only work on gcc-esque compilers.
#if __SIZEOF_LONG_LONG__ > __SIZEOF_LONG__
                case sizeof(long long):
                    arg.as_smax[0] = va_arg(ap, long long);
                break;
#endif

                // long double, complex double, very very extra long int....
                default:
                    arg.as_max_size_type = va_arg(ap, BFMT_MAX_TYPE);
                break;
            }

            bytes_avail -= stream_write(ctx->stream, (char*)&arg.as_ptr[0], bytes_needed);
        }

        args_processed++;
        bytes_needed = ctx->next_arg_bytes(ctx->formatter, NULL);
    }
    return args_processed;
}
