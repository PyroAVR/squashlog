project('os_log', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

includes = include_directories('include')

encoder = executable('encoder',
    files([
        'src' / 'printf_format.c',
        'src' / 'encoder.c',
    ]),
    include_directories: includes,
    install: false
)
