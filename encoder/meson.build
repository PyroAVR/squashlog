project('twig encoder', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

host_cc = meson.get_compiler('c', native: false)

includes = include_directories('include')

lib_packer = static_library('packer',
  files('src' / 'packer.c'),
  include_directories: includes,
  install: false,
  native: false
)
subdir('src' / 'toolchain')
subdir('src' / 'streams')
subdir('src' / 'formatters')

subdir('examples')
