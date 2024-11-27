example_posix_enc = executable('posix_enc',
    files('posix_basic.c'),
    include_directories: [common_includes, encoder_includes],
    link_with: [lib_packer, lib_posix_stream, lib_printf_format],
    link_whole: lib_bfmt,
    install: false,
    native: false
)
