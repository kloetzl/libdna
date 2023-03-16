# dna_ihash_invert(3) - invert a hash function

LIBDNA, 2023-03-16

    #include <kloetzl/dna.h>
    
    uint64_t dna_ihash_invert(uint64_t data);


## Description

Invert a hash value previously created with \fBdna_ihash\fR(). This restores the data stored therein.


## Example

    uint64_t data = 1729;
    uint64_t key  = dna_ihash(data);
    uint64_t recovered = dna_ihash_invert(key);
    assert(data == recovered);


## See Also

[dna_ihash](dna_ihash.3.md)
