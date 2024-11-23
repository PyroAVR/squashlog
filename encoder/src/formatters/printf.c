#include <twig/formatters/printf.h>

#include <twig/packer.h>
#include <twig/streams/byte_queue.h>

#include <stdarg.h>

// TODO printf & friends: MT-Safe locale
// these two static variables should be thread-local storage
// TODO ifdef these behind feature flags if sprintX functions are not needed.
static packer_ctx_t *_packer_ctx;
static char *_printf_ctx;

static int printf_next_arg_bytes(const char **ctx, const char *spec);

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
    int packed_args = 0;
    _printf_ctx = str;
    va_start(ap, fmt);
    packed_args = pack_idx(_packer_ctx, idx);
    packed_args += vpackargs(_packer_ctx, (void*)fmt, ap);
    return packed_args;
}

int _printf(const char *restrict format, int idx, ...) {
    va_list ap;
    va_start(ap, idx);
    int packed_args = 0;
    _packer_ctx->formatter = &format;
    packed_args = pack_idx(_packer_ctx, idx);
    packed_args += vpackargs(_packer_ctx, (void*)format, ap);
    va_end(ap);
    return packed_args;
}

int printf_spec_bytes(const int flags) {
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

/**
 * Consume a printf-style format specifier, returning the number of bytes
 * required to hold the next format argument within spec. Initializes the string
 * context in ctx if spec is non-NULL. Pass NULL for spec to continue parsing
 * the most recent spec string.
 *
 * This function takes one %[flag][width][.][precision][length mod]<conversion>
 * at the start of a format string and returns the number of bytes consumed by
 * that argument before formatting, that is, in binary form, not as text.
 *
 * See man 3 printf for information about these specifiers.
 * spec: a format specifier in printf format, '...%...'
 * returns: number of bytes required for next arg.
 */
static int printf_next_arg_bytes(const char **ctx, const char *spec) {
    int bytes = 0;
    size_t skip = 0;
    int flags = 0;
    // (re)-initialize parsing context if necessary
    if(spec != NULL) {
        *ctx = spec;
    }
    *ctx = printf_skip_non_spec(*ctx);
    flags = printf_spec_flags(*ctx, &skip);
    bytes = printf_spec_bytes(flags);
    *ctx = *ctx + skip;
    return bytes;
}
