# libdna

The aim of this project is to unify functionality commonly found in bioinformatics projects working on DNA. As DNA sequences tend to be long, most functions are designed to be very efficient including SIMD optimizations where appropriate.

# Installation

Libdna requires the Meson buildsystem but no other dependencies.

    meson builddir
    cd builddir
    meson compile
    meson install

# Bonus

- libdna comes with man pages for IUPAC codes and the standard genetic code.
- Some functions pick the optimal SIMD instruction set at runtime.
- To prove efficiency, benchmarks with alternate implementations are included.

# License

Copyright © 2018 - 2021 Fabian Klötzl <kloetzl@evolbio.mpg.de>  
MIT License
