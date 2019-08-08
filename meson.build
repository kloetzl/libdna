project('libdna', 'c', 'cpp', version : '0.2', license : 'GPL3',
    default_options : [
        'warning_level=3',
        'c_std=gnu11',
        'buildtype=debugoptimized',
        'b_ndebug=false' # set to false on non-optimized
    ]
)

if get_option('buildtype').startswith('release')
    warning('library will not be portable')
    # … until I figure out how to discourage meson to use -O3.
endif

###################################################
# libdna

incdir = include_directories('include', 'bench', 'test')

sse42files = ['src/dna4_count_mismatches_sse42.c', 'src/dna4_revcomp_sse42.c']
sse42lib = static_library('sse42lib', sse42files,
    include_directories: incdir,
    pic: true,
    c_args : ['-O3', '-mpopcnt', '-msse4.2']
)

avx2files = ['src/dna4_count_mismatches_avx2.c', 'src/dna4_revcomp_avx2.c']
avx2lib = static_library('avx2lib', avx2files,
    include_directories: incdir,
    pic: true,
    c_args : ['-O3', '-mpopcnt', '-mavx2']
)

avx512files = ['src/dna4_count_mismatches_avx512.c']
avx512lib = static_library('avx512lib', avx512files,
    include_directories: incdir,
    pic: true,
    c_args : ['-O3', '-mpopcnt', '-mavx512bw', '-mavx512vl']
)

genericfiles = [
    'src/dna4_count_mismatches.c',
    # 'src/dna4_count_mismatches_revcomp.c',
    'src/dna4_gc_content.c',
    'src/dna4_pack.c',
    'src/dna4_revcomp.c',
    'src/dna_internal.h',
    'src/dna_version.c',
    'src/dnax_count.c',
    'src/dnax_find_first_not_of.c',
    'src/dnax_find_first_of.c',
    'src/dnax_find_mismatch.c',
    'src/dnax_pack.c',
    'src/dnax_replace.c',
    'src/dnax_revcomp.c',
    'src/dnax_translate.c',
    'src/utils.h'
]

libdna = library('dna', ['include/dna.h', genericfiles],
    include_directories : incdir,
    soversion: '0',
    link_with: [sse42lib, avx2lib, avx512lib]
)

install_headers('include/dna.h')
install_man(['man/geneticcode.7', 'man/iupac.7'])

###################################################
# Tests

testlib = static_library('Tmain', 'test/Tmain.cxx',
    include_directories: incdir,
    cpp_args : ['-O2'],
    install: false,
)

Tsimple = executable('Tsimple', 'test/Tsimple.cxx',
    link_with: [libdna, testlib],
    install: false,
    build_by_default: false,
    include_directories: incdir
)

Tdnax_revcomp = executable('Tdnax_revcomp', 'test/Tdnax_revcomp.cxx',
    link_with: [libdna, testlib],
    install: false,
    build_by_default: false,
    include_directories: incdir
)

Ttranslate = executable('Ttranslate', 'test/Ttranslate.cxx',
    link_with: [libdna, testlib],
    install: false,
    build_by_default: false,
    include_directories: incdir
)

test('Tsimple', Tsimple)
test('Tdnax_revcomp', Tdnax_revcomp)
test('Ttranslate', Ttranslate)

###################################################
# Benchmarks

benchcommonlib = static_library('benchcommon', 'bench/common.cxx',
    include_directories: incdir,
    cpp_args : ['-O2'],
    install: false,
)

libbenchmark = dependency('benchmark',
    # required: false
)

compare = executable('compare', 'bench/compare.cxx',
    link_with: [libdna, benchcommonlib],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    dependencies: libbenchmark,
    cpp_args: ['-Ofast', '-march=native']
)

revcomp = executable('revcomp', 'bench/revcomp.cxx',
    link_with: [libdna, benchcommonlib],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    dependencies: libbenchmark,
    cpp_args: ['-Ofast', '-march=native']
)

compare_rev = executable('compare_rev', 'bench/compare_rev.cxx',
    link_with: [libdna, benchcommonlib],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    dependencies: libbenchmark,
    cpp_args: ['-Ofast', '-march=native']
)

find_first_not = executable('find_first_not', 'bench/find_first_not.cxx',
    link_with: [libdna, benchcommonlib],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    dependencies: libbenchmark,
    cpp_args: ['-Ofast', '-march=native']
)

dnax_to_dna4 = executable('dnax_to_dna4', 'bench/dnax_to_dna4.cxx', 'bench/despace_tables.h',
    link_with: [libdna, benchcommonlib],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    dependencies: libbenchmark,
    cpp_args: ['-Ofast', '-march=native']
)

benchmark('compare', compare)
benchmark('revcomp', revcomp)
benchmark('compare_rev', compare_rev)
benchmark('find_first_not', find_first_not)
benchmark('dnax_to_dna4', dnax_to_dna4)
