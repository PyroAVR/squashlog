#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>
/**
 * Consume a printf-style format specifier.  This function takes one
 * %[flag][width][.][precision][length mod]<conversion> at the start of a format
 * string and returns the number of bytes consumed by that argument before
 * formatting, that is, in binary form, not as text.
 *
 * See man 3 printf for information about these specifiers.
 * fmt: string starting with a format specifier in printf format, '%...'
 * skip: optional. If non-null, pointed value will be set to #chars in fmt str.
 */
int printf_fmt_bytes(char *fmt, int *skip) {
    enum {
        BYTE_MOD_FLAG = 1 << 0,
        SHORT_MOD_FLAG = 1 << 1,
        LONG_MOD_FLAG = 1 << 2,
        LONG_LONG_MOD_FLAG = 1 << 3,
        LONG_DOUBLE_MOD_FLAG = 1 << 4,
        INTMAX_MOD_FLAG = 1 << 5,
        SIZE_T_MOD_FLAG = 1 << 6,
        PTRDIFF_T_MOD_FLAG = 1 << 7,
        INT_CONV_FLAG = 1 << 8,
        DOUBLE_CONV_FLAG = 1 << 9,
        CHAR_CONV_FLAG = 1 << 10,
        STR_CONV_FLAG = 1 << 11,
        PTR_CONV_FLAG = 1 << 12
    };
    int bytes = 0;
    int index = 0;
    bool argwidth_flag = false;
    int flags = 0;
    // consume % sign and specifiers containing only %%, %%%, etc.
    while(fmt[index] && fmt[index] == '%') index++;
    // odd numbered counts of % have a posfix which may be a valid fmtstr
    if(!(index & 1)) goto done;
    // consume flag, if any
    switch(fmt[index]) {
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
    if(fmt[index] == '*') {
        index++; 
        argwidth_flag = true;
    }
    while(fmt[index] && (isdigit(fmt[index]) || fmt[index] == '.')) index++;
    // if the format specifier is malformed, we still try to parse it
    if(argwidth_flag && fmt[index] == '$') index++;

    // length modifiers do matter for this purpose
    switch(fmt[index]) {
        case 'h':
            if(fmt[index + 1] == 'h') {
                flags |= BYTE_MOD_FLAG;
                index += 2;
            }
            else {
                flags |= SHORT_MOD_FLAG;
                index += 1;
            }
        break;
        
        case 'l':
            if(fmt[index + 1] == 'l') {
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

    switch(fmt[index]) {
        case 'd':
        case 'i':
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
            flags |= CHAR_CONV_FLAG;
        break;

        case 'C':
            flags |= CHAR_CONV_FLAG | LONG_MOD_FLAG;
        break;

        case 's':
            flags |= STR_CONV_FLAG;
        break;

        case 'S':
            flags |= STR_CONV_FLAG | LONG_MOD_FLAG;
        break;

        case 'p':
            flags |= PTR_CONV_FLAG;
        break;

        case 'n':
            flags |= PTR_CONV_FLAG;
        break;

    }

    // now, interpret all the flags to determine how many bytes are consumed by
    // this argument
    if(flags & LONG_DOUBLE_MOD_FLAG) {
        bytes = sizeof(long double);
    }
    else if(flags & INTMAX_MOD_FLAG) {
        bytes = sizeof(intmax_t);
    }
    if(flags & SIZE_T_MOD_FLAG) {
        bytes = sizeof(size_t);
    }
    if(flags & PTRDIFF_T_MOD_FLAG) {
        bytes = sizeof(ptrdiff_t);
    }
    if(flags & INT_CONV_FLAG) {
        if(flags & BYTE_MOD_FLAG) {
            bytes = sizeof(char);
        }
        else if(flags & SHORT_MOD_FLAG) {
            bytes = sizeof(short);
        }
        else if(flags & LONG_MOD_FLAG) {
            bytes = sizeof(long);
        }
        else if(flags & LONG_LONG_MOD_FLAG) {
            bytes = sizeof(long long);
        }
        else {
            bytes = sizeof(int);
        }
    }
    if(flags & DOUBLE_CONV_FLAG) {
        bytes = sizeof(double);
    }
    if(flags & CHAR_CONV_FLAG) {
        bytes = (flags & LONG_MOD_FLAG) ? sizeof(wchar_t):sizeof(char);
    }
    if(flags & STR_CONV_FLAG) {
        // have to compute the length of the string pointed to here.
        // because we don't have access to the argument, we return -1 here to
        // indicate use of strlen(), and -2 to indicate use of wcslen()
        bytes = (flags & LONG_MOD_FLAG) ? -2:-1;
    }
    if(flags & PTR_CONV_FLAG) {
        // I don't know of any platforms where the size of pointers to different
        // types changes, but if there are any, this will break.
        bytes = sizeof(void *);
    }
done:
    if(skip != NULL) {
        *skip = index;
    }
    return bytes;
}
