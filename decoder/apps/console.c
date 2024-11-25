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

struct app_ctx {
    bfmt_t host_bfmt;
    array_t *offset_map; // map (index -> strtab offset)
    // XXX not owned by this struct in the current impl.
    char **strtab;
    struct unpacker_ctx *unpackctx;
};

struct app_ctx *app_ctx_init(struct app_ctx *ctx, bfmt_t *host_bfmt, struct unpacker_ctx *unpackerctx, char **strtab) {
    memcpy(&ctx->host_bfmt, host_bfmt, sizeof(bfmt_t));
    
    ctx->offset_map = create_array(1, host_bfmt->pointer_size);
    if(!ctx->offset_map) {
        return NULL;
    }
    ctx->unpackctx = unpackerctx;
    ctx->strtab = strtab;
    return ctx;
}


void read_handler(struct app_ctx *ctx, int fd) {
    printf("read is ready on fd: %i\n", fd);
    ssize_t read_size = 0;
    char buf[256];
    bool new_fmt = true;
    uintmax_t string_idx = 0;
    size_t skip = 0;
    while((read_size = read(fd, buf, 256)) > 0) {
        buf[read_size] = 0; // append null-terminator
        if(new_fmt) {
            // new_fmt = false;
            // get index of this message from first byte & ctx
            string_idx = unpack_idx(&ctx->host_bfmt, buf, &skip);
            printf("index of current message is: %lu\n", string_idx);
        }

        struct tagbox next_arg = {.tag = NO_DATA};
        uint32_t *offset = (uint32_t*)array_fetch(ctx->offset_map, string_idx);
        if(!offset) {
            fprintf(stderr, "Index %i was not found in the string table. Stale input or stream de-sync likely.\n", string_idx);
        }
        else {
            // TODO make a nice macro / func for getting an index into strtab
            // also make one to do the whole lookup, eg. char *fmt = lookup_idx(ctx, idx)
            char *spec = (char*)ctx->strtab + *offset;
            printf("\t%s\n", spec);
        }
        // while((next_arg = unpackgarg(ctx->unpackctx, NULL, buf
        // consume as many arguments as we have available bytes, printing them
        // if all arguments are consumed, new_fmt = true, else new_fmt = false
        // loop
    }
    printf("\n");
}

int main(int argc, const char **argv) {
    if(argc != 5) {
        usage(argv[0]);
        return 1;
    }

    buf_t *strings = buf_from_file(argv[1]);
    if(!strings) {
        goto done_nostrings;
    }
    buf_t *indices = buf_from_file(argv[2]);
    if(!indices) {
        goto done_noindices;
    }
    buf_t *binfmt = buf_from_file(argv[3]);
    if(!binfmt) {
        goto done_nobinfmt;
    }

    bfmt_t *host_bfmt = (bfmt_t*)binfmt->linebuf;


    struct app_ctx ctx;
    app_ctx_init(&ctx, host_bfmt, NULL, (char**)(strings->linebuf));

    printf("host machine has sizeof(void *) = %i\n", host_bfmt->pointer_size);

    printf("host string table:\n");
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
    unsigned int base = 0;
    for(unsigned int i = host_bfmt->pointer_size; i < indices->len; i += host_bfmt->pointer_size) {
        unsigned int strtab_pointer;
        // NOTE u32 works for many platforms, but not guaranteed to be enough for x86.
        memcpy(&strtab_pointer, indices->linebuf + i, sizeof(BFMT_U32_TYPE));
        if(base == 0) {
            base = strtab_pointer;
        }
        strtab_pointer -= base;
        // printf("\toffset of next string: %04x\n", strtab_pointer);
        // printf("\t%i: %s\n", i - host_bfmt->pointer_size, strings->linebuf + strtab_pointer);
        if(ALC_ARRAY_SUCCESS != array_append(ctx.offset_map, (void*)strtab_pointer)) {
            fprintf(stderr, "out of memory during strtab-offset table generation\n");
            goto done_mem;
        }
    }

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
done_mem:
    array_free(ctx.offset_map);
done_nobinfmt:
    buf_free(binfmt);
done_noindices:
    buf_free(indices);
done_nostrings:
    buf_free(strings);
    return 0;
}
