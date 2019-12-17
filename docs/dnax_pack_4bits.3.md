# dnax_pack_4bits(3) - encode DNA using four bits per nucleotide

LIBDNA, 2019-09-19

    #include <dna.h>
    
    size_t dnax_pack_4bits(const char *begin, const char *end, unsigned char *dest);


## Description

The **dnax_pack_4bits**() function packs a string of nucleotides into a sequence of bytes by representing each nucleotide with four bits. The input string is delimited by _begin_ and _end_ (exclusive). The result is stored at _dest_.

The encoding of nucleotides is compatible to the Nucleotide Archival Format. It can represent the common nucleotides, ambiguous bases, uracil (_U_) and gaps (_-_).


## Return Value

The number of encoded characters is returned.


## Example

    const char str[] = "ACGT-W";
    unsigned char buffer[3] = {0};

    size_t nibbles = dnax_pack_4bits(str, str + 6, buffer);
    size_t bytes = (nibbles + 1) &gt;&gt; 1;

    assert(bytes == 3);


## See Also

[dna4_pack_2bits](dna4_pack_2bits.3.md)(3),
[dna4_unpack_4bits](dna4_unpack_4bits.3.md)(3),
[dnax_unpack_4bits](dnax_unpack_4bits.3.md)(3)
