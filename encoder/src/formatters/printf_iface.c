#include <twig/formatters/printf.h>

#include <twig/packer.h>
#include <twig/streams/byte_queue.h>

#include <stdarg.h>

// TODO thread safety... printf & friends are MT-Safe locale
// can I get an f
// TODO ifdef these behind feature flags if sprintX functions are not needed.
static packer_ctx_t *_packer_ctx;
static const char **_printf_ctx;
static queue_t buf_queue;

packer_ctx_t *printf_family_init(packer_ctx_t *target) {
    target->next_arg_bytes = (arg_size_f*)printf_next_arg_bytes;
    target->formatter = (void*)_printf_ctx;
    // TODO separate init from static init
    _packer_ctx = target;
    return target;
}

// NOTE this function overrides packer_ctx->stream.
int _sprintf(char *restrict str, int idx, const char *restrict fmt, ...) {
    va_list ap;
    int packed_bytes = 0;
    _printf_ctx = str;
    va_start(ap, fmt);
    packed_bytes = pack_idx(_packer_ctx, idx);
    packed_bytes += vpackargs(_packer_ctx, (void*)fmt, ap);
    return packed_bytes;
}

int _printf(const char *restrict format, int idx, ...) {
    va_list ap;
    va_start(ap, idx);
    int packed_bytes = 0;
    _packer_ctx->formatter = &format;
    packed_bytes = pack_idx(_packer_ctx, idx);
    packed_bytes += vpackargs(_packer_ctx, (void*)format, ap);
    va_end(ap);
    return packed_bytes;
}
