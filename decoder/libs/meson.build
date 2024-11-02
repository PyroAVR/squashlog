lib_buf = library('fileutils',
  files('file_utils.c'),
  include_directories: decoder_includes,
  install: false,
  native: true
)

lib_epoll = library('epoll',
  files('epoll.c'),
  include_directories: decoder_includes,
  dependencies: [dep_alibc_array, dep_alibc_array_iter],
  install: false,
  native: true
)

lib_unpacker = library('unpacker',
  files('unpacker.c'),
  include_directories: [common_includes, decoder_includes],
  install: false,
  native: true
)
