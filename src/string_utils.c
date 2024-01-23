#include <string_utils.h>

char *strpcpy(char *dst, char *src, size_t len) {
    // naive implementation
    size_t i;
    for(i = 0; i < len; i++) {
        dst[i] = src[i];
        if(src[i] == 0) {
            break;
        }
    }
    return dst + i;
}
