binfmt_file_path = get_variable('binfmt_impl', '')

sources = []

if binfmt_file_path != ''
    sources += [binfmt_file_path]

# attempt autodetection if binary format impl is not overridden
elif host_cc.get_id() in ['gcc', 'llvm']
    sources += ['gcc_like.c']

elif host_cc.get_id() in ['ccrl', 'ccrx']
    sources += ['renesas.c']

else
    error('Compiler type detection failed and no custom implementation provided via binfmt_impl')
endif

lib_bfmt = library('bfmt',
  files(sources),
  native: false,
  include_directories: includes
)
