console_decoder = executable('console-decoder',
    files('console.c'),
    include_directories: [common_includes, decoder_includes],
    link_with: [lib_buf, lib_epoll],
    dependencies: [dep_alibc_array, dep_alibc_array_iter],
    install: false,
    native: true
)
