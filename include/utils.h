#pragma once
/**
 * It's the dreaded utility header!
 */


/**
 * please don't include me everywhere
 */

#define _PASTE(x, y) x##y
#define PASTE(x, y) _PASTE(x, y)

#if defined(__GNUC__) || defined(__clang__)
#define SECTION(name) __attribute__((section(name)))
#define USED __attribute__((used))
#elif defined(_MSC_VER)
#define SECTION(name) #pragma section(name, read)
#elif defined (__CCRL__)
#define SECTION(name) #pragma section const name
#else
#error "Unknown compiler type in " __FILE__ "."
#endif
