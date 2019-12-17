# dnax_unpack_4bits(3) - encode DNA using four bits per nucleotide

LIBDNA, 2019-09-19

    #include <dna.h>
    
    char *dnax_unpack_4bits( const unsigned char *begin, const unsigned char *end, char *dest);


## Description

The **dnax_unpack_4bits**() function decodes the nucleotides previously packed with **dnax_unpack_4bits**(3). The decoding of characters is compatible to the Nucleotide Archival Format.

This function always writes an even number of bytes.


## Return Value

The return value points one byte past the last character written.


## Example

    const char str[] = "ACGT-W";
    unsigned char packed[3] = {0};
    char buffer[7] = {0};

    dnax_pack_4bits(str, str + 6, packed);
    dnax_unpack_4bits(packed, packed + 3, buffer);

    assert(strcmp(str, buffer) == 0);


## See Also

[dna4_pack_2bits](dna4_pack_2bits.3.md)(3),
[dna4_unpack_4bits](dna4_unpack_4bits.3.md)(3),
[dnax_unpack_4bits](dnax_unpack_4bits.3.md)(3)
