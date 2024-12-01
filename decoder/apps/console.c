#include <file-utils.h>
#include <epoll_app.h>
#include <twig/bfmt.h>
#include <twig/unpacker.h>
#include <twig/tagbox.h>

#include <alibc/containers/array.h>

#include <unistd.h>
#include <fcntl.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void usage(const char *name) {
    printf("Usage: %s <strings file> <indices file> <binfmt file> <data stream file>\n", name);
}

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

/**
 * Get a pointer to the string at the specified index
 * NULL is returned if the index is out of bounds.
 */
char* strtab_lookup(const struct strtab *table, uintmax_t index) {
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

struct app_ctx {
    bfmt_t host_bfmt;
    struct strtab *strtab;
    struct unpacker_ctx *unpackctx;
};

struct app_ctx *app_ctx_init(struct app_ctx *ctx, bfmt_t *host_bfmt, struct unpacker_ctx *unpackerctx, struct fbuf *strtab, const struct fbuf *indices) {
    memcpy(&ctx->host_bfmt, host_bfmt, sizeof(bfmt_t));
    
    ctx->unpackctx = unpackerctx;
    ctx->strtab = strtab_init_from_bin(NULL, strtab, indices, host_bfmt);
    return ctx;
}

void read_handler(struct app_ctx *ctx, int fd) {
    // TODO what happens when unpackarg encounters a giant string arg?
    // it should return a pointer, but a pointer to what? There isn't a good
    // way to know what the size is, and if it is larger, into what shall
    // we read it? Perhaps there needs to be a dynabuf in the unpacker context.

    printf("read is ready on fd: %i\n", fd);
    // vars needed for each format string:
    // - read buffer
    // - size of data read
    // - offset into read buffer consumed
    // - spec string
    char buf[256];
    ssize_t bytes_available = 0;
    ssize_t bytes_remaining = 0;
    char *curr_spec = NULL;
    // vars needed for each read chunk:
    // - whether to run unpack_idx (start of new fmt string)
    // - current arg output
    // - bytes to skip
    // - bytes remaining in stream
    bool new_fmt = true;
    struct tagbox next_arg = {.tag = NO_DATA};
    size_t skip = 0;
    size_t bytes_consumed = 0;

    while((bytes_available = read(fd, buf, 256)) > 0) {
        buf[bytes_available] = 0; // append null-terminator
        do {
            if(new_fmt) {
                new_fmt = false;
                // get index of this message from first byte & ctx
                uintmax_t string_idx = unpack_idx(&ctx->host_bfmt, buf, &skip);
                bytes_remaining = bytes_available - skip;
                bytes_consumed = skip;
                curr_spec = strtab_lookup(ctx->strtab, string_idx);
                next_arg = unpackarg(ctx->unpackctx, curr_spec, buf + skip, bytes_available - bytes_remaining);
            }
            else {
                next_arg = unpackarg(ctx->unpackctx, NULL, buf + bytes_consumed, bytes_available - bytes_remaining);
                // TODO advance bytes_consumed and bytes_remaining based on skip value from unpackarg
                // TODO if unpackarg says there are no args left to process, set new_fmt = true
            }
            // do something with next_arg
            // free next_arg when done
            // TODO make read calls deal with a circular buffer / use unpacker's stream context
        while(
    }
    printf("\n");
}

int main(int argc, const char **argv) {
    if(argc != 5) {
        usage(argv[0]);
        return 1;
    }

    struct fbuf *strings = buf_from_file(argv[1]);
    if(!strings) {
        goto done_nostrings;
    }
    struct fbuf *indices = buf_from_file(argv[2]);
    if(!indices) {
        goto done_noindices;
    }
    struct fbuf *binfmt = buf_from_file(argv[3]);
    if(!binfmt) {
        goto done_nobinfmt;
    }

    bfmt_t *host_bfmt = (bfmt_t*)binfmt->linebuf;


    // bfmt is copied, indices is transient, strings is managed by ctx->strtab
    // after this call
    struct app_ctx ctx;
    app_ctx_init(&ctx, host_bfmt, NULL, strings, indices);
    buf_free(indices);
    buf_free(binfmt);
    indices = NULL;
    binfmt = NULL;

    printf("host machine has sizeof(void *) = %i\n", host_bfmt->pointer_size);

    printf("host string table:\n");

    int streamfd = open(argv[4], O_RDONLY);
    if(streamfd == -1) {
        fprintf(stderr, "could not open data stream \"%s\": %s\n", argv[4], strerror(errno));
        goto done;
    }

    read_handler(&ctx, streamfd);
    // no epoll on regular files
#if 0
    struct app_ctx ctx;
    epoll_app_t *epoll = create_epoll_app(0, &ctx);

    int streamfd = open(argv[4], O_RDONLY);
    if(streamfd == -1) {
        fprintf(stderr, "could not open data stream \"%s\": %s\n", argv[4], strerror(errno));
        goto done_nostream;
    }

    epoll->epollin_cb = (epoll_cb_t*)read_handler;

    int res = epoll_app_add_fd(epoll, streamfd, EPOLLIN | EPOLLRDHUP | EPOLLERR);
    if(res) {
        fprintf(stderr, "epoll add failed: %s\n", strerror(errno));
    }

    epoll_app_mainloop(epoll);
    close(streamfd);

done_nostream:
    epoll_app_stop(epoll);
    destroy_epoll_app(epoll);
#endif
done:
done_nobinfmt:
    buf_free(binfmt);
done_noindices:
    buf_free(indices);
done_nostrings:
    buf_free(strings);
    return 0;
}
