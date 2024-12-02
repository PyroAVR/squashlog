#include <file-utils.h>
#include <epoll_app.h>
#include <twig/bfmt.h>
#include <twig/unpacker.h>
#include <twig/tagbox.h>
#include <twig/strtab.h>

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
