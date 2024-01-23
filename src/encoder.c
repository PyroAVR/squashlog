#include "formatters/printf.h"
#include "utils.h"

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

SECTION(OS_LOG_OFFSETS_SECTION) const char *_os_log_fmtstrs_start;
char *sample_fmt_strings[] = {
    "%i%20d%x",
    "this is a string %s with other strings %0.2f inside",
    "this is a string with no format specifiers",
};

char *zero_substr = "";
char *all_text_substr = "this is a substring containing only plaintext";
char *with_fmts = "this is a substring containing %s format %i specifiers";

fmt_ctx_t format_context;
printf_ctx_t printf_context;

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    char big_buf[255] = {0};
    printf_family_init(&format_context, &printf_context);
    xsprintf(&format_context, big_buf, "this is a %s message: %ld", "test", 0xFFFFFF);
    printf("%s\n", big_buf);
    return 0;
}
