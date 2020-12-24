# libdna

The aim of this project is to unify functionality commonly found in bioinformatics projects working on DNA. As DNA sequences tend to be long, most functions are designed to be very efficient including SIMD optimizations where appropriate.

# Installation

Libdna requires the Meson buildsystem but no other dependencies.

    meson builddir
    cd builddir
    ninja
    ninja install


# Bonus

- libdna comes with man pages for IUPAC codes and the standard genetic code.
- Some functions pick the optimal SIMD instruction set at runtime.
- To prove efficiency, benchmarks with alternate implementations are included.

# License

Copyright © 2018 - 2019 Fabian Klötzl <kloetzl@evolbio.mpg.de>  
License GPLv3+: GNU GPL version 3 or later.

This is free software: you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law. The full license text is available at http://gnu.org/licenses/gpl.html.

