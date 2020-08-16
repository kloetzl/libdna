# libdna

The aim of this project is to unify functionality commonly found in bioinformatics projects working on DNA. As DNA sequences tend to be long, most functions are designed to be very efficient including SIMD optimizations where appropriate.

# Installation

Libdna requires the Meson buildsystem but no other dependencies.

    meson builddir
    cd builddir
    ninja
    ninja install

# API

*The following API (and ABI) should not be considered stable for the time being.*

```C
#include <dna.h>
```

Most functions expect a `begin` and an `end` pointer. These are to make the half-open interval of DNA sequence to work on. Instead of relying on null-termination of strings, using an end-pointer enables better SIMD and working only on subsequences of strings. As genomes are long, you should avoid calling `strlen` too often and store the length with the data in one place anyways.

## DNA4

Every symbol exported by this library starts with `dna`. Functions beginning with `dna4_` work on the letters `ACGT` exclusively. All other characters (lower case, null bytes, `U`) may trigger arbitrary behavior.

* dna4_count_mismatches
* dna4_pack_2bits
* dna4_revcomp
* dna4_unpack_2bits

## DNAX

The following functions are not limited to any alphabet. They commonly use a table to allow custom behavior.

* dnax_count
* dnax_extract_dna4
* dnax_find_first_not_of
* dnax_find_first_of
* dnax_find_first_mismatch
* dnax_pack_4bits
* dnax_replace
* dnax_revcomp
* dnax_translate
* dnax_unpack_4bits

# Bonus

- libdna comes with man pages for IUPAC codes and the standard genetic code.
- Some functions pick the optimal SIMD instruction set at runtime.
- To prove efficiency, benchmarks with alternate implementations are included.

# License

Copyright © 2018 - 2020 Fabian Klötzl <kloetzl@evolbio.mpg.de>  
License GPLv3+: GNU GPL version 3 or later.

This is free software: you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law. The full license text is available at http://gnu.org/licenses/gpl.html.

