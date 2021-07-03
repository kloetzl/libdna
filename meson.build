project('libdna', 'c', 'cpp',
    version : '1.0',
    license : 'MIT',
    default_options : ['warning_level=3', 'c_std=gnu11', 'cpp_std=c++17',
        'b_ndebug=if-release', 'buildtype=release']
)

###################################################
# libdna

cc = meson.get_compiler('c')
aversion = meson.project_version().split('.')
major = aversion[0].to_int()
minor = aversion[1].to_int()

configure_file(
    input: 'src/config.h.in',
    output: 'config.h',
    configuration: {
        'CAN_IFUNC': cc.has_function_attribute('ifunc').to_int(),
        'CAN_CONSTRUCTOR': cc.has_function_attribute('constructor').to_int(),
        'MINOR': minor,
        'MAJOR': major
    }
)

incdir = include_directories('include', 'bench', 'test', 'src')

genericfiles = [
    'src/dna4_count_mismatches.c',
    # 'src/dna4_count_mismatches_revcomp.c',
    'src/dna4_pack_2bits.c',
    'src/dna4_revcomp.c',
    'src/dna4_unpack_2bits.c',
    'src/dna_internal.h',
    'src/dna_version.c',
    'src/dnax_count.c',
    'src/dnax_extract_dna4.c',
    'src/dnax_find_first_mismatch.c',
    'src/dnax_find_first_not_of.c',
    'src/dnax_find_first_of.c',
    'src/dnax_pack_4bits.c',
    'src/dnax_replace.c',
    'src/dnax_revcomp.c',
    'src/dnax_translate.c',
    'src/dnax_unpack_4bits.c',
    'src/utils.h'
]

isaspecificlibs = []
if host_machine.cpu_family() == 'x86_64'
    genericfiles += ['src/dna4_count_mismatches_x86.c', 'src/dna4_revcomp_x86.c', 'src/dnax_extract_dna4_x86.c']

    sse42files = ['src/dna4_count_mismatches_sse42.c', 'src/dna4_revcomp_sse42.c', 'src/dnax_extract_dna4_sse42.c']
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

    isaspecificlibs = [sse42lib, avx2lib, avx512lib]
endif

if host_machine.cpu_family() == 'aarch64'
    neonfiles = ['src/dna4_revcomp_neon.c', 'src/dna4_count_mismatches_neon.c']
    neonlib = static_library('neonlib', neonfiles,
        include_directories: incdir,
        pic: true,
        c_args : ['-O3', '-fvisibility=hidden']
    )

    isaspecificlibs = [neonlib]
endif

libdna = library('dna', ['include/dna.h', genericfiles],
    include_directories : incdir,
    soversion: '0',
    link_whole: isaspecificlibs,
    pic: true,
    c_args : ['-O2', '-fvisibility=hidden'],
    install: true
)

install_headers('include/dna.h', 'include/dna', subdir: 'kloetzl')
install_man([
    'man/dna4_count_mismatches.3',
    'man/dna4_pack_2bits.3',
    'man/dna4_revcomp.3',
    'man/dna4_unpack_2bits.3',
    'man/dnax_count.3',
    'man/dnax_extract_dna4.3',
    'man/dnax_find_first_mismatch.3',
    'man/dnax_find_first_not_of.3',
    'man/dnax_find_first_of.3',
    'man/dnax_pack_4bits.3',
    'man/dnax_replace.3',
    'man/dnax_revcomp.3',
    'man/dnax_translate.3',
    'man/dnax_unpack_4bits.3',
    'man/geneticcode.7',
    'man/iupac.7'
])

# simplify using this as a subproject for other meson users
libdna_dep = declare_dependency(include_directories : 'include',
    link_with : libdna)

# create pkgconf file for users of this lib
pkg = import('pkgconfig')
pkg.generate(
    libraries : libdna,
    subdirs : ['.', 'kloetzl'],
    name : 'libdna',
    description : 'Essential Functions for DNA Manipulation'
)

###################################################
# Tests
# meson setup builddir_wtests -Dwith-tests=true

if get_option('with-tests')

    catch2_dep = dependency('catch2', fallback: ['catch2', 'catch2_dep'])

    testlib = static_library('Tmain', 'test/Tmain.cxx',
        include_directories: incdir,
        cpp_args : ['-O2'],
        install: false,
        dependencies: catch2_dep
    )

    nativetests = ['Tdna4_pack_2bits', 'Tdna4_revcomp']
    foreach Tname : nativetests
        Texe = executable(Tname,
            'test/' + Tname + '.cxx',
            link_with: [testlib, libdna],
            install: false,
            build_by_default: false,
            include_directories: incdir,
            cpp_args: ['-march=native'],
            dependencies: catch2_dep
        )
        test(Tname, Texe)
    endforeach

    plaintests = [
        'Tcompare',
        'Tdnax_pack_4bits',
        'Tdnax_revcomp',
        'Tsimple',
        'Ttranslate',
        'Tdnax_extract_dna4'
    ]
    foreach Tname : plaintests
        Texe = executable(Tname,
            'test/' + Tname + '.cxx',
            link_with: [testlib, libdna],
            install: false,
            build_by_default: false,
            include_directories: incdir,
            dependencies: catch2_dep
        )
        test(Tname, Texe)
    endforeach

endif

###################################################
# Benchmarks
# meson setup builddir_wbench -Dwith-benchmarks=true

if get_option('with-benchmarks')

    benchcommonlib = static_library('benchcommon', 'bench/Bcommon.cxx',
        include_directories: incdir,
        cpp_args : ['-O2'],
        install: false
    )

    libbenchmark = dependency('benchmark',
            fallback: ['google-benchmark', 'google_benchmark_dep'])

    plainbench = [
        'Bcompare',
        'Bcompare_wg',
        'Bcompare_rev',
        'Bmismatch',
        'Brevcomp',
        'Bstrconstr'
    ]
    foreach Bname : plainbench
        Bexe = executable(Bname, 'bench/' + Bname + '.cxx',
            link_with: [libdna, benchcommonlib],
            install: false,
            build_by_default: true,
            include_directories: incdir,
            dependencies: libbenchmark,
            cpp_args: ['-Ofast', '-march=native']
        )
        benchmark(Bname, Bexe)
    endforeach

    Bdnax_to_dna4 = executable('Bdnax_to_dna4', 'bench/Bdnax_to_dna4.cxx', 'bench/despace_tables.h',
        link_with: [libdna, benchcommonlib],
        install: false,
        build_by_default: true,
        include_directories: incdir,
        dependencies: libbenchmark,
        cpp_args: ['-Ofast', '-march=native']
    )

    benchmark('Bdnax_to_dna4', Bdnax_to_dna4)

endif
