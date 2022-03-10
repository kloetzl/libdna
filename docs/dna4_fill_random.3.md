# dna4_fill_random(3) - fill a buffer with random nucleotides

LIBDNA, 2022-03-10

    #include <kloetzl/dna.h>
    
    void dna4_fill_random(char *begin, char *end, uint32_t seed);


## Description

The **dna4\_fill\_random**() function fills a buffer with random nucleotides (ACGT). A _seed_ can be supplied to select a different random sequence. The buffer is delimited by _begin_ and _end_ (exclusive). 


## Example

    char buffer[11] = {0};
    dna4_fill_random(buffer, buffer + 10, 23);
    printf("%s\n", buffer);


## See Also

[iupac](iupac.7.md)

