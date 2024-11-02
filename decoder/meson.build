decoder_includes = [include_directories('include')]
dep_alibc_array = dependency('alibc_array', fallback: ['alibc.containers', 'dep_array'], native: true)
dep_alibc_array_iter = dependency('alibc_array_iter', fallback: ['alibc.containers', 'dep_array_iter'], native: true)

subdir('libs')

if get_option('build_demos')
  subdir('apps')
endif
