#include <twig/formatters/printf.h>
#include <twig/streams/posix.h>
#include <twig/compiler.h>
#include <twig/packer.h>

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

SECTION(TWIG_OFFSETS_SECTION) const char *_twig_fmtstrs_start;

packer_ctx_t format_context;
posix_stream_t stream;

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // TODO, check that printf & friends aren't duplicating information re: _printf_ctx.
    printf_family_init(&format_context);
    posix_stream_init(&format_context, posix_stream_from_file(&stream, stdout));
    xprintf("hello %s, %i\n", "world", 14);
    return 0;
}
