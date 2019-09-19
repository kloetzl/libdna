project('libdna', 'c', 'cpp', version : '0.2', license : 'GPL3',
    default_options : [
        'warning_level=3',
        'c_std=gnu11',
        'buildtype=debugoptimized',
        'optimization=2',
        'b_ndebug=if-release'
    ]
)

warning(get_option('buildtype'))
if get_option('buildtype').startswith('release')
endif

###################################################
# libdna

incdir = include_directories('include', 'bench', 'test', 'src')

sse42files = ['src/dna4_count_mismatches_sse42.c', 'src/dna4_revcomp_sse42.c']
sse42lib = static_library('sse42lib', sse42files,
    include_directories: incdir,
    pic: true,
    c_args : ['-O3', '-mpopcnt', '-msse4.2', '-fvisibility=hidden']
)

avx2files = ['src/dna4_count_mismatches_avx2.c', 'src/dna4_revcomp_avx2.c']
avx2lib = static_library('avx2lib', avx2files,
    include_directories: incdir,
    pic: true,
    c_args : ['-O3', '-mpopcnt', '-mavx2', '-fvisibility=hidden']
)

avx512files = ['src/dna4_count_mismatches_avx512.c']
avx512lib = static_library('avx512lib', avx512files,
    include_directories: incdir,
    pic: true,
    c_args : ['-O3', '-mpopcnt', '-mavx512bw', '-mavx512vl', '-fvisibility=hidden']
)

genericfiles = [
    'src/dna4_count_mismatches.c',
    # 'src/dna4_count_mismatches_revcomp.c',
    'src/dna4_gc_content.c',
    'src/dna4_pack_2bits.c',
    'src/dna4_revcomp.c',
    'src/dna4_unpack_2bits.c',
    'src/dna_internal.h',
    'src/dna_version.c',
    'src/dnax_count.c',
    'src/dnax_find_first_not_of.c',
    'src/dnax_find_first_of.c',
    'src/dnax_find_mismatch.c',
    'src/dnax_pack_4bits.c',
    'src/dnax_replace.c',
    'src/dnax_revcomp.c',
    'src/dnax_translate.c',
    'src/dnax_unpack_4bits.c',
    'src/utils.h'
]

libdna = library('dna', ['include/dna.h', genericfiles],
    include_directories : incdir,
    soversion: '0',
    link_with: [sse42lib, avx2lib, avx512lib],
    pic: true,
    c_args : ['-O2', '-fvisibility=hidden']
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

Tdna4_pack_2bits = executable('Tdna4_pack_2bits', 'test/Tdna4_pack_2bits.cxx',
    link_with: [testlib, libdna],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    cpp_args: ['-march=native']
)

Tdna4_revcomp = executable('Tdna4_revcomp', 'test/Tdna4_revcomp.cxx',
    link_with: [testlib, libdna],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    cpp_args: ['-march=native']
)

Tdnax_pack_4bits = executable('Tdnax_pack_4bits', 'test/Tdnax_pack_4bits.cxx',
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

Tsimple = executable('Tsimple', 'test/Tsimple.cxx',
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

test('Tdna4_pack_2bits', Tdna4_pack_2bits)
test('Tdna4_revcomp', Tdna4_revcomp)
test('Tdnax_pack_4bits', Tdnax_pack_4bits)
test('Tdnax_revcomp', Tdnax_revcomp)
test('Tsimple', Tsimple)
test('Ttranslate', Ttranslate)

###################################################
# Benchmarks

benchcommonlib = static_library('benchcommon', 'bench/Bcommon.cxx',
    include_directories: incdir,
    cpp_args : ['-O2'],
    install: false,
)

libbenchmark = dependency('benchmark',
    # required: false
)

Bcompare = executable('Bcompare', 'bench/Bcompare.cxx',
    link_with: [libdna, benchcommonlib],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    dependencies: libbenchmark,
    cpp_args: ['-Ofast', '-march=native']
)

Brevcomp = executable('Brevcomp', 'bench/Brevcomp.cxx',
    link_with: [libdna, benchcommonlib],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    dependencies: libbenchmark,
    cpp_args: ['-Ofast', '-march=native']
)

Bcompare_rev = executable('Bcompare_rev', 'bench/Bcompare_rev.cxx',
    link_with: [libdna, benchcommonlib],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    dependencies: libbenchmark,
    cpp_args: ['-Ofast', '-march=native']
)

Bfind_first_not = executable('Bfind_first_not', 'bench/Bfind_first_not.cxx',
    link_with: [libdna, benchcommonlib],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    dependencies: libbenchmark,
    cpp_args: ['-Ofast', '-march=native']
)

Bdnax_to_dna4 = executable('Bdnax_to_dna4', 'bench/Bdnax_to_dna4.cxx', 'bench/despace_tables.h',
    link_with: [libdna, benchcommonlib],
    install: false,
    build_by_default: false,
    include_directories: incdir,
    dependencies: libbenchmark,
    cpp_args: ['-Ofast', '-march=native']
)

benchmark('Bcompare', Bcompare)
benchmark('Brevcomp', Brevcomp)
benchmark('Bcompare_rev', Bcompare_rev)
benchmark('Bfind_first_not', Bfind_first_not)
benchmark('Bdnax_to_dna4', Bdnax_to_dna4)
