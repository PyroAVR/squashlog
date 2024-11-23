encoder_includes = include_directories('include')

encoder_defines = {
  'TWIG_HOST_HAS_WCHAR_H': host_cc.check_header('wchar.h'),
  'TWIG_BUILD_TARGETS_HOST': 1,
}
encoder_c_args = []
foreach key,value: encoder_defines
  encoder_c_args += '-D' + key + '=' + value.to_string()
endforeach


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
