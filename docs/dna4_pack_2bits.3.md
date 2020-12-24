# dna4_pack_2bits(3) - encode DNA using two bits per nucleotide

LIBDNA, 2019-09-19

    #include <kloetzl/dna.h>
    
    uint64_t dna4_pack_2bits(const char *begin, size_t k);


## Description

The **dna4_pack_2bits**() function packs a _k_-mer into one 64bit integer using two bits per nucleotide. Thus, _k_ is limited to 32.

The result is undefined if the input string contains characters besides
_A_,
_C_,
_G_ and
_T_.
Lowercase letters are also not allowed.


## Return Value

One packed 64bit integer representing the _k_ input nucleotides.


## Example

    const char str[] = "ACGTACGTTTCC";
    uint64_t packed = dna4_pack_2bits(str, 12);

    assert(packed == 0x1b1bf5);


## See Also

[dna4_unpack_2bits](dna4_unpack_2bits.3.md), 
[dnax_pack_4bits](dnax_pack_4bits.3.md), 
[dnax_unpack_4bits](dnax_unpack_4bits.3.md)
