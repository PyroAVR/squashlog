#include <twig/strtab.h>

#include <alibc/containers/array.h>

#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h> // FIXME remove or improve debug logging

struct strtab *strtab_init_from_bin(struct strtab *table, struct fbuf *strtab, const struct fbuf *indices, const bfmt_t *host_bfmt) {
    if(table == NULL) {
        table = malloc(sizeof(struct strtab));
        if(table == NULL) {
            fprintf(stderr, "Out of memory while allocating strtab\n");
            goto done;
        }
        memset((void*)table, 0, sizeof(struct strtab));
    }

    // clear old entries
    if(table->offset_map != NULL) {
        // XXX HAX: there is no "clear" function in alc_array :(
        table->offset_map->size = 0;
    }
    else {
        // array of offsets in host pointer size
        table->offset_map = create_array(1, host_bfmt->pointer_size);
        if(!table->offset_map) {
            goto done_free_table;
        }
    }

    // populate offsets into strtab from indices
    if(table->strtab != NULL) {
        buf_free(table->strtab);
    }
    table->strtab = strtab;

    // this loop increments a pointer and subtracts an offset, but using
    // host pointer size.
    // this algorithm accounts for the initial zero in the string index table.
    // While it makes the decoder slightly ugly, it drastically simplifies the
    // logic on the encoder side, which is dependent on macros.
    //
    // The first entry in the index table is bogus data (likely zero), to
    // account for the _twig_stridx_start - a dummy allocation that will have
    // an address at the beginning of the index table. The second entry is the
    // starting address of the strtab, which becomes the variable "base" here.
    // All future entries are offset by base, so we subtract base from them
    // to get the real offset into the binary strtab file.
    // Were the executable loaded as ELF or similar format, the actual address
    // would be meaningful as a loadaddr of the strtab.
    uintmax_t base = 0;
    for(unsigned int i = host_bfmt->pointer_size; i < indices->len; i += host_bfmt->pointer_size) {
        uintmax_t strtab_pointer;
        memcpy(&strtab_pointer, indices->linebuf + i, host_bfmt->pointer_size);
        if(base == 0) {
            base = strtab_pointer;
        }
        strtab_pointer -= base;
        if(ALC_ARRAY_SUCCESS != array_append(table->offset_map, (void*)strtab_pointer)) {
            fprintf(stderr, "out of memory during strtab-offset table generation\n");
            goto done_free_array;
        }
    }
    
    goto done; // skip de-init
done_free_array:
    array_free(table->offset_map);
done_free_table:
    free(table);
    table = NULL;
done:
    return table;
}
const char* strtab_lookup(const struct strtab *table, const uintmax_t index) {
    char *spec = NULL;
    uintmax_t *offset = (uintmax_t*)array_fetch(table->offset_map, index);
    if(!offset) {
        fprintf(stderr, "Index %lu was not found in the string table. Stale input or stream de-sync likely.\n", index);
    }
    else {
        spec = (char*)table->strtab->linebuf + *offset;
    }
    return spec;
}

void strtab_free(struct strtab *table) {
    if(table) {
        buf_free(table->strtab);
        array_free(table->offset_map);
        free(table);
    }
}
