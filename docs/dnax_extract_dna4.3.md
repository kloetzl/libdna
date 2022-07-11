# dnax_extract_dna4(3) - convert to dna4

LIBDNA, 2020-06-08

    #include <kloetzl/dna.h>
    
    char *dnax_extract_dna4(const char *begin, const char *end, char *dest);


## Description

The **dnax\_extract\_dna4**() function converts lower case characters to upper case and only keeps ACGT. It also converts U and u to T. The input string is delimited by _begin_ and _end_ (exclusive). The result is stored at _dest_.

## Return Value

The return value points one byte past the last character written. You may want to write a null byte there.

## Example

    char in[] = "AaC!GT";
    char out[6] = {0};
    
    char *end = dnax_extract_dna4(in, in + 6, out);
    *end = '\0';
    
    assert(strncmp(out, "AACGT", 5) == 0);

## See Also

[iupac](iupac.7.md), [dnax_replace](dnax_replace.3.md)
