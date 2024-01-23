#pragma once
#include "formatter.h"
#include "os_log.h" // hax...

#undef sprintf
#define xsprintf(ctx, buf, fmt, ...) {\
    int offset;\
    OS_LOG_FMT_NAME(fmt, offset);\
    _sprintf(ctx, buf, offset, fmt, __VA_ARGS__);\
}

typedef struct printf_ctx_S printf_ctx_t;

/**
 * Yield bytes for each format specifier in a printf-style string
 */
int printf_fmt_r(char *fmt_spec, printf_ctx_t *ctx);

struct printf_ctx_S {
    char *format_str;
    int index;
};

/**
 * Initialize the formatter context for printf-family formatters
 */
fmt_ctx_t *printf_family_init(fmt_ctx_t *target, printf_ctx_t *ctx);

int _sprintf(fmt_ctx_t *ctx, char *restrict str, int idx, const char *restrict fmt, ...);
