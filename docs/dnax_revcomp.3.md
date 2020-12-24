# dnax_revcomp(3) - compute the reverse complement

LIBDNA, 2019-08-12

    #include <dna.h>
    
    char *dnax_revcomp(const char *table, const char *begin, const char *end, char *dest);


## Description

The **dnax_revcomp**() function computes the reverse complement. The input string is delimited by _begin_ and _end_ (exclusive). The result is stored at _dest_.

The _table_ parameter can be used to customize the reverse complement of individual characters. The supplied table _dnax_revcomp_table_ complements according to the IUPAC notation. A value of -1 can be used to indicate a character which should be skipped. Thus, the resulting string may be shorter than the input.


## Return Value

The return value points one byte past the last character written. You may want to write a null byte there.


## Example

    const char str[] = "ACGTacgtACGT!";
    char buffer[13];
    char *end = dnax_revcomp(dnax_revcomp_table, str, str + sizeof(str) - 1, buffer);
    *end = '\\0';

    assert(strncmp(str, buffer, 12) == 0);


## See Also

[dna4_revcomp](dna4_revcomp.3.md)(3),
[iupac](iupac.7.md)(7)
