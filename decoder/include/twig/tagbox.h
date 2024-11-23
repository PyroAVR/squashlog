#pragma once

#include <twig/box.h>
/**
 * A box to hold any scalar type, with a tag to define how to handle it.
 * "Scalar" in this context defines any type that is understood by C, and in
 * addition includes complex doubles and null-terminated strings.
 * The tag refers to the decoder's understanding of size
 */
struct tagbox {
    enum {
        AS_CHAR,
        AS_INT,
        AS_S8,
        AS_S16,
        AS_S32,
        AS_S64,
        AS_SMAX,
        AS_U8,
        AS_U16,
        AS_U32,
        AS_U64,
        AS_UMAX,
        AS_PTR,
        AS_FLOAT,
        AS_DOUBLE,
        AS_LONG_DOUBLE, // TODO complex ?= long double, size wise
        AS_CSTRING
    } tag;
    union {
        union box data;
        char *str;
    };
};
