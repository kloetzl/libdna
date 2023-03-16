# libdna

The aim of this project is to unify functionality commonly found in bioinformatics projects working on DNA. DNA, as opposed to RNA or amino acid sequences, are very long strings. Even bacterial genomes are easily a few megabyte in size. Thus, for efficient analysis the length has to be taken into account in the design of an application. To this end, libdna contains SIMD routines highly optimised for DNA strings. For some functions the library even chooses the optimal implementation depending on the CPU at runtime.

# Installation

Libdna requires the [Meson](https://mesonbuild.com/) buildsystem. It is commonly available via package managers. Then execute the following steps to compile and install the latest version of libdna.

    git clone https://github.com/kloetzl/libdna.git
    meson builddir
    cd builddir
    meson compile
    meson install

Contributors may also want to take a look at the `Makefile.Maintainer`. It contains handy shortcuts set up tests, benchmarks and other release related files.

# How to use

Libdna is both simple, efficient and customizable. For instance, many bioinformatics tools need to compute the reverse complement of some DNA sequence. Now it is just one function call away, `dna4_revcomp`. The prefix `dna4` indicates that this function is optimised for strings containing only the four canonical nucleotides A, C, G and T. The first parameter is a pointer to the beginning of the string. The second parameter points to the first byte just past the string. So for a string starting at `str` of length `len` the arguments are `str` and `str + len` representing the string `[str, str+1, …, str+len)`. The last parameter is a pointer to a location with enough space to hold the reverse compliment.

```C
#include <kloetzl/dna.h>

int main()
{
    char buffer[] = "ACGT";
    char rev[5] = {0};
    dna4_revcomp(buffer, buffer + 4, rev);

    printf("%s\n", rev);
}
```

In C++ things are even simpler thanks to a thin wrapper. Instead of raw pointers it uses `std::string_view` and `std::string` to make the API more convenient.

```C++
#include <kloetzl/dna.hpp>

int main()
{
    std::cout << dna4::revcomp("ACGT") << "\n";
}
```

As the wrapper relies on automatic memory management which can incur a significant runtime overhead, the underlying C functions are still available for use. Don't forget to link with `-ldna`.

# Bonus

- libdna comes with man pages for IUPAC codes and the standard genetic code.
- Where a `dna4_` function provides high performance at the expense of limited applicability the `dnax_` functions provide generality at the expense of speed.
- Some functions pick the optimal SIMD instruction set at runtime.
- To prove efficiency, benchmarks with alternate implementations are included.

# License

Copyright © 2018 - 2023 Fabian Klötzl <fabian-libdna@kloetzl.info>  
MIT License
