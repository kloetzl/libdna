# dna4_count_mismatches_rc(3) - count the number of mismatching bytes in revcomp

LIBDNA, 2022-03-10

    #include <kloetzl/dna.h>

    size_t dna4_count_mismatches_rc(const char *begin, const char *end, const char *other);


## Description

The **dna4\_count\_mismatches\_rc**() function efficiently compares two dna strings across strands and returns the number of mismatching bytes. The first string is delimited by _begin_ and _end_ (exclusive). For the second string only the start must be supplied via _other_ as it is assumed to be at least as long as the former. 

The result is undefined if the input strings contain characters besides
_A_,
_C_,
_G_ and
_T_.
Lowercase letters are also not allowed. Null bytes are not interpreted as the end of a string.


## Return Value

Returns the number of mismatches of two string on opposite strands.


## Example

    const char seq1[] = "TAACCGCCCTTGGG";
    const char seq2[] = "CCCAAGTGCGATTA";
    size_t snps = dna4_count_mismatches_rc(seq1, seq1 + sizeof(seq1) - 1, seq2);

    assert(snps == 2);


## See Also

[dnax_count_mismatches](dnax_count_mismatches.3.md)
[dna4_revcomp](dna4_revcomp.3.md)
