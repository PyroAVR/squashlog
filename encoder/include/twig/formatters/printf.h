#pragma once

#include <twig/packer.h>
#include <twig/compiler.h>

#define xprintf(dest, fmt, ...) {\
    int offset;\
    TWIG_MKSTR(fmt, offset);\
    _printf(dest, offset, fmt, __VA_ARGS__);\
}

/**
 * Initialize the formatter context for printf-family formatters
 */
packer_ctx_t *printf_family_init(packer_ctx_t *target);

// TODO compiler abstraction
//__attribute__((section(".init_array")))
//void printf_internal_init(void);

int _printf(const char *restrict format, int idx, ...);

int _sprintf(char *restrict str, int idx, const char *restrict format, ...);

// internal functions
/**
 * Parse spec, returning the number of bytes required to hold the next format
 * argument within spec. Initializes the string context in ctx if spec is
 * non-NULL. Pass NULL for spec to continue parsing the most recent spec string.
 * spec: format-spec string in printf style
 * returns: number of bytes required for next arg.
 */
int printf_next_arg_bytes(const char **ctx, const char *spec);
