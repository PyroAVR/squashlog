encoder_includes = include_directories('include')

lib_packer = static_library('packer',
  files('src' / 'packer.c'),
  include_directories: [encoder_includes, common_includes],
  install: false,
  native: false
)
subdir('src' / 'toolchain')
subdir('src' / 'streams')
subdir('src' / 'formatters')

if get_option('build_demos')
  subdir('demos')
endif
