#pragma once

#include <stddef.h>

/**
 * String functions
 */

// TODO determine where this header should go.
// TODO obsolete this header eventually by not doing any string copies :)
//      can we send an offset into the stringtab where all the strings provided
//      will be? Likely not, but it's worth a shot.


/**
 * Somewhere in between strncpy and stpncpy.
 * Returns a pointer to the 1 + the last character copied from src.
 * At most dst + len, at least dst.
 */
char *strpcpy(char *dst, char *src, size_t len);
