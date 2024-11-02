#include <twig/formatters/printf.h>
#include <twig/formatters/printf_layer.h>
#include <twig/streams/posix.h>
#include <twig/compiler.h>
#include <twig/packer.h>

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

// TODO this is marked extern in compiler.h so that it is not different on a
// per-object basis during linking (would cause offsets to be wrong).
// It would be nice to define this in the depths of the library somewhere so
// that it isn't necessary to put it in user code, but that may not be possible
// to do while keeping twig a library that can be linked static but not compiled
// as one object.
SECTION(TWIG_OFFSETS_SECTION) const char *_twig_stridx_start;

packer_ctx_t format_context;
posix_stream_t stream;

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // TODO, check that printf & friends aren't duplicating information re: _printf_ctx.
    printf_family_init(&format_context);
    posix_stream_init(&format_context, posix_stream_from_file(&stream, stdout));
    xprintf("hello %s, %i\n", "world", 14);
    xprintf("floating point number: %g\n", 3.141592);
    return 0;
}
