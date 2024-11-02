binfmt_file_path = get_variable('binfmt_impl', '')

sources = []

if binfmt_file_path != ''
    sources += [binfmt_file_path]
    # TODO allow for overriding this in options
    objcopy_prog = find_program('objcopy', required: false)

# attempt autodetection if binary format impl is not overridden
elif host_cc.get_id() in ['gcc', 'llvm']
    sources += ['gcc_like.c']
    objcopy_prog = find_program('objcopy', required: false)
    # TODO make the section names map to the cpp defs
    objcopy_bfmt_cmd = ['-O', 'binary', '-j', '.binfmt_data']
    objcopy_idxs_cmd = ['-O', 'binary', '-j', '.twig.stridxs']
    objcopy_strs_cmd = ['-O', 'binary', '-j', '.twig.strtab']

else
    error('Compiler type detection failed and no custom implementation provided via binfmt_impl')
endif

lib_bfmt = static_library('bfmt',
  files(sources),
  native: false,
  include_directories: [encoder_includes, common_includes],
  build_by_default: true,
)

if objcopy_prog.found()
endif
