project('squashlog', 'c',
  version : '0.1',
  default_options : ['warning_level=3']
)

host_cc = meson.get_compiler('c', native: false)

includes = include_directories('include')

encoder_source_list = [
    'src' / 'formatters' / 'printf.c',
    'src' / 'formatters' / 'packer.c',
    'src' / 'encoder.c',
    'src' / 'string_utils.c',
]

# TODO not really sure if this is a good way to allow inclusion as a subproject.
binfmt_file_path = get_variable('binfmt_impl', '')

if binfmt_file_path != ''
    encoder_source_list += [binfmt_file_path]
elif host_cc.get_id() in ['gcc', 'llvm']
    encoder_source_list += ['src' / 'binfmt_gcc_like.c']
elif host_cc.get_id() in ['ccrl', 'ccrx']
    encoder_source_list += ['src' / 'binfmt_renesas.c']
else
    error('Compiler type detection failed and no custom implementation provided via binfmt_impl')
endif


encoder_exe = executable('encoder',
    files(encoder_source_list),
    include_directories: includes,
    install: false,
    native: false
)


objcopy_prog = find_program('objcopy')
binfmt_always_args = ['-O', 'binary', '-j', '.binfmt_data']
string_idx_always_args = ['-O', 'binary', '-j', '.os_log_fmtstrs']
# TODO not all toolchains / linker scripts will put constant strings here.
# need a config option
strings_always_args = ['-O', 'binary', '-j', '.rodata']

binfmt_file = custom_target('binfmt',
    output: 'binfmt.bin',
    input: encoder_exe,
    command: [objcopy_prog, binfmt_always_args, '@INPUT@', '@OUTPUT@'],
    install: false,
    build_by_default: true
)

string_idx_file = custom_target('string_idxs',
    output: 'string_idxs.bin',
    input: encoder_exe,
    command: [objcopy_prog, string_idx_always_args, '@INPUT@', '@OUTPUT@'],
    install: false,
    build_by_default: true
)

strings_file = custom_target('strings',
    output: 'strings.bin',
    input: encoder_exe,
    command: [objcopy_prog, strings_always_args, '@INPUT@', '@OUTPUT@'],
    install: false,
    build_by_default: true
)

decoder_source_list = [
    'src' / 'decoder.c',
    'src' / 'string_extract.c',
]

decoder_exe = executable('decoder',
    files(decoder_source_list),
    include_directories: includes,
    install: false,
    native: true
)
