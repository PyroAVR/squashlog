#include <twig/formatters/printf.h>

#include <twig/packer.h>
#include <twig/streams/byte_queue.h>

#include <stdarg.h>

// TODO thread safety... printf & friends are MT-Safe locale
// can I get an f
// TODO ifdef these behind feature flags if sprintX functions are not needed.
static packer_ctx_t *_packer_ctx;
static char *_printf_ctx;

packer_ctx_t *printf_family_init(packer_ctx_t *target) {
    target->next_arg_bytes = (arg_size_f*)printf_next_arg_bytes;
    target->formatter = (void*)_printf_ctx;
    // TODO separate init from static init
    _packer_ctx = target;
    return target;
}

// NOTE this function overrides packer_ctx->stream.
int _sprintf(char *restrict str, int idx, const char *restrict fmt, ...) {
    va_list ap;
    int packed_bytes = 0;
    _printf_ctx = str;
    va_start(ap, fmt);
    packed_bytes = pack_idx(_packer_ctx, idx);
    packed_bytes += vpackargs(_packer_ctx, (void*)fmt, ap);
    return packed_bytes;
}

int _printf(const char *restrict format, int idx, ...) {
    va_list ap;
    va_start(ap, idx);
    int packed_bytes = 0;
    _packer_ctx->formatter = &format;
    packed_bytes = pack_idx(_packer_ctx, idx);
    packed_bytes += vpackargs(_packer_ctx, (void*)format, ap);
    va_end(ap);
    return packed_bytes;
}

int printf_spec_bytes_flags(const int flags) {
    int bytes = 0;
    if(flags & LONG_DOUBLE_MOD_FLAG) {
        bytes = sizeof(long double);
    }
    else if(flags & INTMAX_MOD_FLAG) {
        bytes = sizeof(intmax_t);
    }
    if(flags & SIZE_T_MOD_FLAG) {
        bytes = sizeof(size_t);
    }
    if(flags & PTRDIFF_T_MOD_FLAG) {
        bytes = sizeof(ptrdiff_t);
    }
    if(flags & INT_CONV_FLAG) {
        if(flags & BYTE_MOD_FLAG) {
            bytes = sizeof(char);
        }
        else if(flags & SHORT_MOD_FLAG) {
            bytes = sizeof(short);
        }
        else if(flags & LONG_MOD_FLAG) {
            bytes = sizeof(long);
        }
        else if(flags & LONG_LONG_MOD_FLAG) {
            bytes = sizeof(long long);
        }
        else {
            bytes = sizeof(int);
        }
    }
    if(flags & DOUBLE_CONV_FLAG) {
        bytes = sizeof(double);
    }
    if(flags & CHAR_CONV_FLAG) {
        bytes = (flags & LONG_MOD_FLAG) ? sizeof(wchar_t):sizeof(char);
    }
    if(flags & STR_CONV_FLAG) {
        // have to compute the length of the string pointed to here.
        // because we don't have access to the argument, we return -1 here to
        // indicate use of strlen(), and -2 to indicate use of wcslen()
        bytes = (flags & LONG_MOD_FLAG) ? -2:-1;
    }
    if(flags & PTR_CONV_FLAG) {
        // I don't know of any platforms where the size of pointers to different
        // types changes, but if there are any, this will break.
        bytes = sizeof(void *);
    }
    return bytes;
}
