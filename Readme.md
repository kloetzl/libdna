# libdna

The aim of this project is to unify functionality commonly found in bioinformatics projects working on DNA. As DNA sequences tend to be long, most functions are designed to be very efficient including SIMD optimizations where appropriate.

# API

*The following API (and ABI) should not be considered stable for the time being.*

```C
#include <dna.h>
```

Most functions expect a `begin` and an `end` pointer. These are to make the half-open interval of DNA sequence to work on. Instead of relying on null-termination of strings, using an end-pointer enables better SIMD and working only on subsequences of strings. As genomes are long, you should avoid calling `strlen` too often and store the length with the data in one place anyways.

## Naming Convention

Every symbol exported by this library starts with `dna`. Functions beginning with `dna4_` work on the letters `ACGT` exclusively. All other characters (lower case, null bytes, `U`) may trigger arbitrary behavior. If your sequences contain other data you want to use the functions provided by `dnax_`. They generally use a table to allow custom behavior.

```C
double dna4_evodist_jc(const char *begin, const char *end, const char *other, size_t *substitutions);
```

`dna4_evodist_jc` computes the number of substitutions between the two sequences starting at `begin` and `other` (both of minimum length `end - begin`). If `substitutions` is not a null pointer, it will get assigned the raw number of mismatches. The return value is the Jukes-Cantor corrected evolutionary distance. Link with `-lm` for increased numeric precision.


```C
double dna4_gc_content(const char *begin, const char *end);
```

This function returns the relative amount of `C` and `G` in the given string.


```C
uint64_t dna4_hash(const char *begin, size_t k);
```

A very common routine is to reduce a k-mer to a simple number where each nucleotide is represented by just two bits. This function does that for a k up to 32.

```C
char *dna4_revcomp(const char *begin, const char *end, char *dest);
```

`dna4_revcomp` implements a very fast way to compute the reverse complement. `dest` is required to hold `end-begin` many characters. The end of the reverse complement is returned.

```C
uint64_t dnax_hash(const char *table, const char *begin, size_t k);
```

Compute the hash of a k-mer. Each entry in the table should contribute two bits to the resulting value (eg. `table['T'] = 3`). A value of -1 can be used to indicate that this letter should be skipped; Note this required `begin` to be followed by more than `k` characters. A naive hash function applicable to DNA and RNA is available as `dnax_hash_table`.

```C
char *dnax_revcomp(const char *table, const char *begin, const char *end, char *dest);
```

Compute the reverse complement of a sequence and store the result int `dest`. The table is used to define the complement. The complement as defined by the IUPAC (plus `U`) is available as `dnax_revcomp_table`. The return value is the end of the reverse complement.


```C
void dnax_count(size_t *table, const char *begin, const char *end);
```

Store the number of occurrences for individual characters in the given table.

```C
char *dnax_replace(const char *table, const char *begin, const char *end, char *dest);
```

Copy a string from `begin` to `dest`, replace each character by the one defined in the table. A value of -1 indicates that a character should be skipped. Having the destination and the source string equal is allowed. To convert RNA to DNA and remove all incompletely specified nucleotides use `dnax_to_dna4_table`.

# Bonus

- libdna comes with man pages for IUPAC codes and the standard genetic code.
- Some functions pick the optimal SIMD instruction set at runtime.
- To prove efficiency, benchmarks with alternate implementations are included.

# License

Copyright © 2018 Fabian Klötzl <kloetzl@evolbio.mpg.de>  
License GPLv3+: GNU GPL version 3 or later.

This is free software: you are free to change and redistribute it. There is NO WARRANTY, to the extent permitted by law. The full license text is available at http://gnu.org/licenses/gpl.html.

