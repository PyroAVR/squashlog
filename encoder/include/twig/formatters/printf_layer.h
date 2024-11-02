#pragma once

#include <twig/packer.h>

#define xprintf(fmt, ...) {\
    int offset;\
    TWIG_MKSTR(fmt, offset);\
    _printf(fmt, offset, __VA_ARGS__);\
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
