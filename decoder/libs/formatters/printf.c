#include <twig/formatters/printf.h>

#include <twig/bfmt.h>

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <ctype.h>

int printf_spec_bytes(const int flags, bfmt_t *host_bfmt) {
    int bytes = 0;
    if(flags & LONG_DOUBLE_MOD_FLAG) {
        bytes = host_bfmt->long_double_size;
    }
    else if(flags & INTMAX_MOD_FLAG) {
        bytes = host_bfmt->intmax_size;
    }
    if(flags & SIZE_T_MOD_FLAG) {
        bytes = host_bfmt->size_t_size;
    }
    if(flags & PTRDIFF_T_MOD_FLAG) {
        bytes = host_bfmt->ptrdiff_t_size;
    }
    if(flags & INT_CONV_FLAG) {
        if(flags & BYTE_MOD_FLAG) {
            bytes = host_bfmt->char_size;
        }
        else if(flags & SHORT_MOD_FLAG) {
            bytes = host_bfmt->short_size;
        }
        else if(flags & LONG_MOD_FLAG) {
            bytes = host_bfmt->long_size;
        }
        else if(flags & LONG_LONG_MOD_FLAG) {
            bytes = host_bfmt->long_long_size;
        }
        else {
            bytes = host_bfmt->int_size;
        }
    }
    if(flags & DOUBLE_CONV_FLAG) {
        bytes = host_bfmt->double_size;
    }
    if(flags & CHAR_CONV_FLAG) {
        bytes = (flags & LONG_MOD_FLAG) ? host_bfmt->wchar_t_size:host_bfmt->char_size;
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
        bytes = host_bfmt->pointer_size;
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
 * ctx: printf decoder context
 * spec: a format specifier in printf format, '...%...'
 * returns: number of bytes required for next arg.
 */
int printf_next_arg_bytes(struct printf_ctx *ctx, const char *spec) {
    int bytes = 0;
    size_t skip = 0;
    int flags = 0;
    // (re)-initialize parsing context if necessary
    if(spec != NULL) {
        ctx->spec = spec;
    }
    ctx->spec = printf_skip_non_spec(ctx->spec);
    flags = printf_spec_flags(ctx->spec, &skip);
    bytes = printf_spec_bytes(flags, ctx->host_bfmt);
    ctx->spec += skip;
    return bytes;
}
