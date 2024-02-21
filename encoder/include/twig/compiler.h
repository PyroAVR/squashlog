#pragma once

#include <twig/config.h>

#include <stdint.h>

#if defined(__GNUC__) || defined(__clang__)
#define SECTION(name) __attribute__((section(name)))
#define USED __attribute__((used))
#elif defined(_MSC_VER)
#define SECTION(name) #pragma section(name, read)
#define USED
#elif defined (__CCRL__)
#define SECTION(name) #pragma section const name
#define USED
#else
#error "Unknown compiler type in " __FILE__ "."
#endif

// create zeroth entry in the format string section, denoting the beginning
extern SECTION(TWIG_OFFSETS_SECTION) const char *_twig_fmtstrs_start;

#define _PASTE(x, y) x##y
#define PASTE(x, y) _PASTE(x, y)

/**
 * Create a new string from a constant literal which:
 *  - resides within TWIG_DATA_SECTION
 *  - has a pointer within TWIG_OFFSETS_SECTION pointing to its first character
 *  - has no effect on the containing scope.
 *
 * Place the index of the offset within TWIG_OFFSETS_SECTION in out_id
 */
#define TWIG_MKSTR(str, out_id) {\
    SECTION(TWIG_DATA_SECTION)\
    static const char _[] = str;\
    SECTION(TWIG_OFFSETS_SECTION)\
    static const char *PASTE(_TWIG_, __LINE__) = _;\
    out_id = (uintptr_t)(&PASTE(_TWIG_, __LINE__) - &_twig_fmtstrs_start);\
}

// example usage of MKSTR TODO delete
#define twig(fmt, ...) {\
    int offset;\
    TWIG_MKSTR(fmt, offset);\
    printf("%i: " fmt, offset, __VA_ARGS__);\
}

#undef PASTE
#undef _PASTE
