#pragma once

#include <twig/bfmt.h>
#include <file-utils.h>

#include <alibc/containers/array.h>

#include <stdint.h>

struct strtab {
    struct fbuf *strtab;
    array_t *offset_map;
};

/**
 * Create a strtab from a file path, allocating memory where necessary.
 * Pass NULL for table to allocate all memory. Otherwise, any NULL field will
 * be filled in as appropriate.
 *
 * When this function returns non-NULL, the strtab argument is owned by the
 * resulting structure and will be freed when reassigned or the structure is
 * destroyed.
 */
struct strtab *strtab_init_from_bin(struct strtab *table, struct fbuf *strtab, const struct fbuf *indices, const bfmt_t *host_bfmt);

/**
 * Get a pointer to the string at the specified index
 * NULL is returned if the index is out of bounds.
 */
const char* strtab_lookup(const struct strtab *table, const uintmax_t index);

void strtab_free(struct strtab *table);
