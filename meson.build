project('twig', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

host_cc = meson.get_compiler('c', native: false)

alibc = subproject('alibc.containers')

subdir('common')
subdir('encoder')
subdir('decoder')
