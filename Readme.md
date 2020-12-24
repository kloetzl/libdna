# libdna

[![Build Status](https://travis-ci.org/kloetzl/libdna.svg?branch=master)](https://travis-ci.org/kloetzl/libdna)
[![Documentation Status](https://readthedocs.org/projects/libdna/badge/?version=latest)](https://libdna.readthedocs.io/en/latest/?badge=latest)

The aim of this project is to unify functionality commonly found in bioinformatics projects working on DNA. As DNA sequences tend to be long, most functions are designed to be very efficient, including SIMD optimizations where appropriate.

# Installation

Libdna requires the Meson buildsystem.

    meson builddir --buildtype=release
    cd builddir
    ninja
    ninja install

# How to use

Libdna is both simple, efficient and customizable. For instance, many bioinformatics tools need to compute the reverse complement of some sequence. Now it is just one function call away.

```C
#include <dna.h>

int main()
{
	char buffer[] = "ACGT";
	char rev[5] = {0};
	dna4_revcomp(buffer, buffer + 4, rev);

	printf("%s\n", rev);
}
```

In C++ things are even simpler thanks to a thin wrapper.

```C++
#include <dna>

int main()
{
	std::cout << dna4::revcomp("ACGT") << "\n";
}
```

Don't forget to link with `-ldna`.

# Bonus

- libdna comes with man pages for IUPAC codes and the standard genetic code.
- Some functions pick the optimal SIMD instruction set at runtime.
- To prove efficiency, benchmarks with alternate implementations are included.

# License

Copyright © 2018 - 2020 Fabian Klötzl <kloetzl@evolbio.mpg.de>  
License GPLv3+: GNU GPL version 3 or later.

This is free software: you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law. The full license text is available at http://gnu.org/licenses/gpl.html.
