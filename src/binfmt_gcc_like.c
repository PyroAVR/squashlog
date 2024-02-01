#include "platform/binformat.h"

#include "utils.h"

#include <limits.h>

SECTION(".binfmt_data")
USED
static const binfmt_t binfmt_defs = {
    .magic = BINFMT_MAGIC,
    .version = BINFMT_VERSION,
    .byte_order_32 = BINFMT_ENDIANNESS_DETECT32,
    .byte_order_16 = BINFMT_ENDIANNESS_DETECT16,
    .char_bit = CHAR_BIT,
    .double_size = __SIZEOF_DOUBLE__,
    .long_double_size = __SIZEOF_LONG_DOUBLE__,
    .float_size = __SIZEOF_FLOAT__,
    .size_t_size = __SIZEOF_SIZE_T__,
    .wint_t_size = __SIZEOF_WINT_T__,
    .pointer_size = __SIZEOF_POINTER__,
    .ptrdiff_size = __SIZEOF_PTRDIFF_T__,
    .long_size = __SIZEOF_LONG__,
    .int_size = __SIZEOF_INT__,
};
