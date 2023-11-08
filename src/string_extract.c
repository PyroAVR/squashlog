#include "string_extract.h"

binfmt_data_t binfmt_yield_next_r(char *fmt, char *ctx) {
    binfmt_data_t r;

    return r;
}

void yield_next_fmtstr(char *fmt) {
    while(*fmt) {
        int skip = 0;
        int bytes = 0;
        if(*fmt == '%') {
            bytes = printf_fmt_bytes(fmt, &skip);
            printf("got format string: %s with arg length: %i\n", fmt, bytes);
            fmt += skip;
        }
        else {
            fmt++;
        }
    }
}
