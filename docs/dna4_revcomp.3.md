# dna4_revcomp(3) - compute the reverse complement

LIBDNA, 2019-08-12

    #include <dna.h>
    
    char *dna4_revcomp(const char *begin, const char *end, char *dest);


## Description

The **dna4_revcomp**() function efficiently computes the reverse complement. The input string is delimited by _begin_ and _end_ (exclusive). The result is stored at _dest_.

The result is undefined if the input string contains characters besides
_A_,
_C_,
_G_ and
_T_.
Lowercase letters are also not allowed.


## Return Value

The return value points one byte past the last character written (i.e. _dest + end - begin_). You may want to write a null byte there.


## Example

    const char str[] = "ACGTACGTACGT";
    char buffer[13];
    char *end = dna4_revcomp(str, str + sizeof(str) - 1, buffer);
    *end = '\\0';
    
    assert(strncmp(str, buffer, 12) == 0);


## See Also

[dnax_revcomp](dnax_revcomp.3.md)
