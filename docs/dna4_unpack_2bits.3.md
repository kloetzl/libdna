# dna4_unpack_2bits(3) - decode two-bit packed DNA representation

LIBDNA, 2019-09-19

    #include <dna.h>
    
    "void dna4_unpack_2bits(char *" dest ", size_t " k ", uint64_t " packed ");"


# Description

Given a packed _k_-mer the **dna4_pack_2bits**() function recovers its ASCII representation. The resulting string is written to the location pointed to by _dest_.


# Example

.in +4
.EX
uint64_t packed = 0x1b1bf5;
char buffer[13] = {0};
dna4_unpack_2bits(buffer, 12, packed);

assert(strcmp(buffer, "ACGTACGTTTCC") == 0);


# See Also

**dna4_pack_2bits**(3)
**dnax_pack_4bits**(3)
**dnax_unpack_4bits**(3)
