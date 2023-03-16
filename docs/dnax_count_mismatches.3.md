# dnax_count_mismatches(3) - count the number of mismatching bytes

LIBDNA, 2023-03-16

    #include <kloetzl/dna.h>
    
    size_t dnax_count_mismatches(const char *begin, const char *end, const char *other);


## Description

The **dnax_count_mismatches**() function efficiently compares two strings and returns the number of mismatching bytes. The first string is delimited by _begin_ and _end_ (exclusive). For the second string only the start must be supplied via _other_ as it is assumed to be at least as long as the former. Null bytes are not interpreted as the end of a string.


## Return Value

Returns the number of mismatches.


## Example

    const char seq1[] = "ACGTACGTACGT";
    const char seq2[] = "ACGTTCGTACGA";
    size_t snps = dnax_count_mismatches(seq1, seq1 + sizeof(seq1) - 1, seq2);

    assert(snps == 2);
