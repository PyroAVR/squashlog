#pragma once

/**
 * Section in which to place format strings used with twig.
 */
#if !defined(TWIG_DATA_SECTION)
#define TWIG_DATA_SECTION ".twig.strtab"
#endif

/**
 * Name of the section to use for the offsets into the TWIG_DATA_SECTION which
 * are the start points of each format specifier
 */
#if !defined(TWIG_OFFSETS_SECTION)
#define TWIG_OFFSETS_SECTION ".twig.stridxs"
#endif

/**
 * Maximum size of the index needed to represent the number of offsets in
 * TWIG_OFFSETS_SECTION. Default is 3, which gives 65k offsets. The additional
 * byte is for the msgpack type code.
 */
#if !defined(TWIG_HUGE_FMTTBL)
#define TWIG_HUGE_FMTTBL 0
#endif

#if TWIG_HUGE_FMTTBL == 1
#define TWIG_IDX_SZ_MAX 9U
#else
#define TWIG_IDX_SZ_MAX 3U
#endif
