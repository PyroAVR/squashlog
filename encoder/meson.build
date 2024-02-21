project('twig encoder', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

host_cc = meson.get_compiler('c', native: false)

includes = include_directories('include')

packer_sources = []
subdir('src' / 'toolchain')
subdir('src' / 'streams')
subdir('src' / 'formatters')

packer_sources += [files('src' / 'packer.c')]

lib_packer = library('packer',
  packer_sources,
  include_directories: includes,
  install: false,
  native: false
)

subdir('examples')
