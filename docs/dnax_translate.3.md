# dnax_translate(3) - translate DNA or RNA to amino acids

LIBDNA, 2020-06-08

    #include <dna.h>

    char *dnax_translate(const char *begin, const char *end, char *dest);


# Description

The **dnax\_translate**() function translates DNA and RNA to an amino acid sequence using the standard genetic code. The input string is delimited by _begin_ and _end_ (exclusive). The result is stored at _dest_.


# Return Value

The return value points one byte past the last character written. You may want to write a null byte there.


# Example

    char in[] = "AUG!CTN";
    char out[3] = {0};

    char *end = dnax_translate(in, in + 7, out);
    *end = '\\0';

    assert(strncmp(out, "ML", 3) == 0);


# See Also

**iupac**(7),
**geneticcode**(7)
