#include <twig/formatters/printf.h>

#include <twig/bfmt.h>

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#if defined(TWIG_HOST_HAS_WCHAR_H)
#include <wchar.h>
#endif

int printf_spec_flags(const char *spec, size_t *skip) {
    int index = 0;
    bool argwidth_flag = false;
    int flags = 0;

    // consume % sign and specifiers containing only %%, %%%, etc.
    while(spec[index] && spec[index] == '%') index++;
    // even-length sequences of "%" are just escaped "%" - ignore.
    // if there are no bytes left in the format string, exit.
    if(!(index & 1) || (index == 0)) goto done;
    // consume flag, if any
    switch(spec[index]) {
        case '#':
        case '0':
        case '-':
        case ' ':
        case '+':
        case '\'':
        case 'I':
            index++;
        break;
        default:
        break;
    }
    // consume field width, if any
    // this is for *m$ argument-width notation
    if(spec[index] == '*') {
        index++; 
        argwidth_flag = true;
    }
    while(spec[index] && (isdigit(spec[index]) || spec[index] == '.')) index++;
    // if the format specifier is malformed, we still try to parse it
    if(argwidth_flag && spec[index] == '$') index++;

    // length modifiers do matter for this purpose
    switch(spec[index]) {
        case 'h':
            if(spec[index + 1] == 'h') {
                flags |= BYTE_MOD_FLAG;
                index += 2;
            }
            else {
                flags |= SHORT_MOD_FLAG;
                index += 1;
            }
        break;
        
        case 'l':
            if(spec[index + 1] == 'l') {
                flags |= LONG_LONG_MOD_FLAG;
                index += 2;
            }
            else {
                flags |= LONG_MOD_FLAG;
                index += 1;
            }
        break;

        case 'q':
            flags |= LONG_LONG_MOD_FLAG;
            index++;
        break;

        case 'L':
            flags |= LONG_DOUBLE_MOD_FLAG;
            index++;
        break;

        case 'j':
            flags |= INTMAX_MOD_FLAG;
            index++;
        break;

        case 'z':
        case 'Z':
            flags |= SIZE_T_MOD_FLAG;
            index++;
        break;

        case 't':
            flags |= PTRDIFF_T_MOD_FLAG;
            index++;
        break;
    }

    switch(spec[index]) {
        case 'd':
        case 'i':
            flags |= INT_CONV_FLAG | UNSIGNED_FLAG;
        break;

        case 'o':
        case 'u':
        case 'x':
        case 'X':
            flags |= INT_CONV_FLAG;
        break;

        case 'e':
        case 'E':
        case 'f':
        case 'F':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
            flags |= DOUBLE_CONV_FLAG;
        break;

        case 'c':
            flags |= CHAR_CONV_FLAG | UNSIGNED_FLAG;
        break;

        case 'C':
            flags |= CHAR_CONV_FLAG | LONG_MOD_FLAG | UNSIGNED_FLAG;
        break;

        case 's':
            flags |= STR_CONV_FLAG;
        break;

        case 'S':
            flags |= STR_CONV_FLAG | LONG_MOD_FLAG | UNSIGNED_FLAG;
        break;

        case 'p':
            flags |= PTR_CONV_FLAG;
        break;

        case 'n':
            flags |= PTR_CONV_FLAG;
        break;

    }
done:
    if(skip != NULL) {
        *skip = index;
    }
    return flags;
}

const char *printf_skip_non_spec(const char *spec) {
    while(*spec != '%' && *spec != 0) spec++;
    return spec;
}
