# API

*The following API (and ABI) should not be considered stable for the time being.*

```C
#include <dna.h>
```

Most functions expect a `begin` and an `end` pointer. These are to make the half-open interval of DNA sequence to work on. Instead of relying on null-termination of strings, using an end-pointer enables better SIMD and working only on subsequences of strings. As genomes are long, you should avoid calling `strlen` too often and store the length with the data in one place anyways.

## DNA4

Every symbol exported by this library starts with `dna`. Functions beginning with `dna4_` work on the letters `ACGT` exclusively. All other characters (lower case, null bytes, `U`) may trigger arbitrary behavior.

* [dna4_count_mismatches](dna4_count_mismatches.3.md)
* [dna4_revcomp](dna4_revcomp.3.md)
* [dna4_pack_2bits](dna4_pack_2bits.3.md)
* [dna4_unpack_2bits](dna4_unpack_2bits.3.md)

## DNAX

The following functions are not limited to any alphabet. They commonly use a table to allow custom behavior.

* [dnax_pack_4bits](dnax_pack_4bits.3.md)
* [dnax_unpack_4bits](dnax_unpack_4bits.3.md)
* [dnax_revcomp](dnax_revcomp.3.md)
* [dnax_translate](dnax_translate.3.md)
* [dnax_count](dnax_count.3.md)
* [dnax_replace](dnax_replace.3.md)
* [dnax_find_mismatch](dnax_find_mismatch.3.md)
* [dnax_find_first_of](dnax_find_first_of.3.md)
* [dnax_find_first_not_of](dnax_find_first_not_of.3.md)
* [dnax_find_first_not_dna4](dnax_find_first_not_dna4.3.md)
