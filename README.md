# Squash-log
`printf` on embedded systems is either expensive, or done through a semihosting
library, usually coupled to a particular set of debug tools. In addition,
normally the onus is on the embedded processor to format the string, making
debug prints costly, and useless for debugging timing related bugs.

This library is neither coupled to a debug tool, nor does it perform formatting.

 - Encoding of the log data is linear with respect to the number of total bytes
   of **arguments**, only.
 - Zero mandatory heap usage.
 - Bounded stack usage.
 - Toolchain agnostic.
 - Minimal-copy design, no-copy possible with DMA.
 - No impact on rodata size in the loaded firmware binary.
 - Wire endianness is native for the embedded CPU.


## How is this accomplished?
Generally speaking, `printf` and its family process the format string bytewise,
calling stringifiers for each vararg according to the format specifiers in the
format string.
In the case of the string-backed versions (`snprintf` and friends),
stringification, string (re)allocation, and numerous copies are involved.
Depending on the underlying system, file-backed versions (`fprintf` and co.)
may or may not need to perform all of the above steps.

All of the above steps are expensive on embedded systems, and thus this library
attempts to minimize them.

### Stringification
Is not performed on the embedded device. The binary contents of format arguments
are copied without formatting into the message packet.

### Memory (re)allocation
Is not performed directly by this library. Companion libraries, if used, may be
used to perform memory re-allocation if necessary, such as in an RTOS message
buffer task. Memory usage is bounded by arguments passed into the library,
ensuring that the user is in full control.

### Copies
Are minimized through careful construction of buffers. Buffers are always
densely packed and bounds checks reduce the chance of overrun and undefined
behavior drastically. Logically separate functions minimize the information
passed between each other to reduce duplication of information.

The format string itself is not transmitted, which removes at least one implicit
string copy. Format strings are indexed at compile time, and only the index is
transmitted.


## Encoding / Wire format
TODO

## Decoding
TODO

## Extracting the binary format information

### From ELF format (GCC, LLVM, modern versions of CC-RL & IAR EW)
In general `objdump` and `objcopy` are the tools of choice for this task, though
their equivalents in proprietary toolchains may be needed in certain
environments.

Modern versions of CC-RL appear to be a thinly-veiled LLVM toolchain and they
also output ELF, just with the ".abs" extension. IAR `xlink` produces ELF,
older versions of `ilink` do not, but IAR may be able to provide a conversion
utility.

### From COFF format (MSVC)
TODO

### From Mach-O format (Apple-branded LLVM & GCC)
TODO

### From Alien Computers
If, somehow, you find yourself using this utility on a system which does not
support ELF, COFF, or Mach-O... please email the developer.
The easiest way to extract the binary format information will be to produce a
modified version of the firmware / application binary which prints these tables
to a file or a serial port, and then capture the result with non-alien computer.
The tables can then be loaded directly by the decoder.
