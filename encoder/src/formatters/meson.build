lib_printf_format = static_library('printf_format',
  files([
    'printf.c',
  ]) + printf_parse_common_sources,
  native: false,
  link_with: [lib_packer, lib_bfmt],
  include_directories: [common_includes, encoder_includes]
)
