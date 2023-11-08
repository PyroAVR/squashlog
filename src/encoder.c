#include "os_log.h"
#include "utils.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

char *sample_fmt_strings[] = {
    "%i%20d%x",
    "this is a string %s with other strings %0.2f inside",
    "this is a string with no format specifiers",
};

// encoding will require va_list. Has to be interepreted one arg at a time
// to support serialization of pointer arguments, namely strings.
// additionally, although it is likely that the arguments for a va_list are
// consecutive, alignment is an issue on embedded platforms and contiguity is
// not guaranteed by ANSI or ISO C.
char *zero_substr = "";
char *all_text_substr = "this is a substring containing only plaintext";
char *with_fmts = "this is a substring containing %s format %i specifiers";

// context is just a pointer into the fmt string
typedef char* os_log_ctx_t;

int os_log_pack_idx(uintmax_t value, char *buffer) {
    int size = 9; // worst case result is 9 bytes.
    if((value & 0x7FULL) == value) {
        // value < 127
        size = 1;
        *buffer = value & 0x7F; // clear bit 7 to zero
    }
    else if((value & 0xFFULL) == value) {
        size = 2;
        buffer[0] = (char)0xCC; // 8-bit int format
        buffer[1] = value & 0xFF;
    }
    else if((value & 0xFFFFULL) == value) {
        size = 3;
        buffer[0] = (char)0xCD; // 16-bit int format
        buffer[1] = value & 0xFF;
        buffer[2] = (value & (0xFF << 8) >> 8);
    }
#if defined(OS_LOG_HUGE_FMTTBL) && OS_LOG_HUGE_FMTTBL == 1
    else if((value & 0xFFFFFFFFULL) == value) {
        size = 5;
        buffer[0] = 0xCE; // 32-bit int format
        buffer[1] = value & 0xFF;
        buffer[2] = (value & (0xFF << 8) >> 8);
        buffer[3] = (value & (0xFFUL << 16) >> 16);
        buffer[4] = (value & (0xFFUL << 24) >> 24);
    }
    else {
        buffer[0] = 0xCF; // 64-bit int format
        buffer[1] = value & 0xFF;
        buffer[2] = (value & (0xFF << 8) >> 8);
        buffer[3] = (value & (0xFFUL << 16) >> 16);
        buffer[4] = (value & (0xFFUL << 24) >> 24);
        buffer[5] = (value & (0xFFULL << 32) >> 32);
        buffer[6] = (value & (0xFFULL << 40) >> 40);
        buffer[7] = (value & (0xFFULL << 48) >> 48);
        buffer[8] = (value & (0xFFULL << 56) >> 56);
    }
#else
    else {
        size = 0; // return 0 size to indicate error
    }
#endif
    return size;
}


int _os_log_snprintf(char *buffer, size_t size, int offset, char *fmt, ...) {
    va_list ap;
    size_t fmt_idx = 0;
    size_t buf_idx = 0;
    union {
        uintmax_t temp_buf;
        char *end;
    } locals;

    buf_idx = os_log_pack_idx(offset, buffer);
    va_start(ap, fmt);
    while(fmt[fmt_idx] && fmt_idx < size && buf_idx < size) {
        int skip = 0;
        int bytes = 0;
        if(fmt[fmt_idx] == '%') {
            bytes = printf_fmt_bytes(fmt + fmt_idx, &skip);
        }
        if(skip != 0) {
            // bytes is valid
            if(buf_idx + bytes > size) {
                // exit early, no space in buffer
                goto done;
            }
            // NOTE this might need to be platform-specific.
            // in theory the standard says that variadic arguments always
            // receive a promoted copy, so this switch *should* work anywhere.
            switch(bytes) {
                case sizeof(char):
                case sizeof(short int):
                case sizeof(int):
                case sizeof(void *):
                    // any integer type, this works when the platform is ilp32
                    locals.temp_buf = va_arg(ap, int);
                    memcpy(buffer + buf_idx, &locals.temp_buf, bytes);
                break;

                case -1:
                    // string specifier
                    locals.end = stpncpy(buffer + buf_idx, va_arg(ap, char *), size - buf_idx);
                    bytes = locals.end - (buffer + buf_idx);
                break;

                case -2:
                    // wide strings not supported yet.
                default:
                break;

            }
            buf_idx += bytes;
            fmt_idx += skip + 1;
        }
        else {
            // bytes is invalid, not a format spec. continue.
            fmt_idx++;
        }
    }
done:
    return buf_idx;
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

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    char big_buf[255];
    os_log_s(big_buf, 255, "this is a %s message: %ld", "test", 0xFFFFFF);
    printf("%s\n", big_buf);
    for(int i = 0; i < 3; i++) {
        yield_next_fmtstr(sample_fmt_strings[i]);
    }
    return 0;
}
