project('libdna', 'c', 'cpp', version : '0.2', license : 'GPL3')

incdir = include_directories('include')
genericfiles = [
    'src/dna4_evodist_jc.c',
    'src/dna4_evodist_k80.c',
    'src/dna4_gc_content.c',
    'src/dna4_pack.c',
    'src/dna4_revcomp.c',
    'src/dna_version.c',
    'src/dnax_count.c',
    'src/dnax_find_first_not_of.c',
    'src/dnax_find_first_of.c',
    'src/dnax_pack.c',
    'src/dnax_mismatch.c',
    'src/dnax_replace.c',
    'src/dnax_revcomp.c',
    'src/dnax_translate.c',
    'src/utils.c'
]

sse42files = ['src/dna4_evodist_jc_sse42.c', 'src/dna4_revcomp_sse42.c']
sse42lib = static_library('sse42lib', sse42files,
    include_directories: incdir,
    pic: true,
    c_args : ['-O3', '-mpopcnt', '-msse4.2']
)

avx2files = ['src/dna4_evodist_jc_avx2.c']
avx2lib = static_library('avx2lib', avx2files,
    include_directories: incdir,
    pic: true,
    c_args : ['-O3', '-mpopcnt', '-mavx2']
)

avx512files = ['src/dna4_evodist_jc_avx512.c']
avx512lib = static_library('avx512lib', avx512files,
    include_directories: incdir,
    pic: true,
    c_args : ['-O3', '-mpopcnt', '-mavx512bw', '-mavx512vl']
)

library('dna', ['include/dna.h', genericfiles],
    include_directories : incdir,
    soversion: '0',
    link_with: [sse42lib, avx2lib, avx512lib]
)

install_headers('include/dna.h')
install_man(['man/geneticcode.7', 'man/iupac.7'])
